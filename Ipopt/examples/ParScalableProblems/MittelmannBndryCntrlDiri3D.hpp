// Copyright (C) 2005, 2009 International Business Machines and others.
// All Rights Reserved.
// This code is published under the Common Public License.
//
// $Id$
//
// Authors:  Andreas Waechter             IBM    2005-10-18
//           Olaf Schenk   (Univ. of Basel)      2007-08-01
//              modified MittelmannBndryCntrlDiri.hpp for 3-dim problem

#ifndef __MITTELMANNBNDRYCNTRLDIRI3D_HPP__
#define __MITTELMANNBNDRYCNTRLDIRI3D_HPP__

#include "RegisteredTNLP.hpp"

#ifdef HAVE_CMATH
# include <cmath>
#else
# ifdef HAVE_MATH_H
#  include <math.h>
# else
#  error "don't have header file for math"
# endif
#endif

#include <cstdio>

using namespace Ipopt;

/** Base class for boundary control problems with Dirichlet boundary
 *  conditions, as formulated by Hans Mittelmann as Examples 1-4 in
 *  "Optimization Techniques for Solving Elliptic Control Problems
 *  with Control and State Constraints. Part 2: Boundary Control"
 *
 *  Here, the control variables are identical to the values of y on
 *  the boundary, and therefore we don't need any explicit
 *  optimization variables for u.
 */
class MittelmannBndryCntrlDiriBase3D : public RegisteredTNLP
{
public:
  /** Constructor. */
  MittelmannBndryCntrlDiriBase3D();

  /** Default destructor */
  virtual ~MittelmannBndryCntrlDiriBase3D();

  /**@name Overloaded from TNLP */
  //@{
  /** Method to return some info about the nlp */
  virtual bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                            Index& nnz_h_lag, IndexStyleEnum& index_style);

  /** Method to return the bounds for my problem */
  virtual bool get_bounds_info(Index n, Number* x_l, Number* x_u,
                               Index m, Number* g_l, Number* g_u);

  /** Method to return the starting point for the algorithm */
  virtual bool get_starting_point(Index n, bool init_x, Number* x,
                                  bool init_z, Number* z_L, Number* z_U,
                                  Index m, bool init_lambda,
                                  Number* lambda);

  /** Method to return the objective value */
  virtual bool eval_f(Index n, const Number* x, bool new_x, Number& obj_value);

  /** Method to return the gradient of the objective */
  virtual bool eval_grad_f(Index n, const Number* x, bool new_x, Number* grad_f);

  /** Method to return the constraint residuals */
  virtual bool eval_g(Index n, const Number* x, bool new_x, Index m, Number* g);

  /** Method to return:
   *   1) The structure of the jacobian (if "values" is NULL)
   *   2) The values of the jacobian (if "values" is not NULL)
   */
  virtual bool eval_jac_g(Index n, const Number* x, bool new_x,
                          Index m, Index nele_jac, Index* iRow, Index *jCol,
                          Number* values);

  /** Method to return:
   *   1) The structure of the hessian of the lagrangian (if "values" is NULL)
   *   2) The values of the hessian of the lagrangian (if "values" is not NULL)
   */
  virtual bool eval_h(Index n, const Number* x, bool new_x,
                      Number obj_factor, Index m, const Number* lambda,
                      bool new_lambda, Index nele_hess, Index* iRow,
                      Index* jCol, Number* values);

  //@}

  /** Method for returning scaling parameters */
  virtual bool get_scaling_parameters(Number& obj_scaling,
                                      bool& use_x_scaling, Index n,
                                      Number* x_scaling,
                                      bool& use_g_scaling, Index m,
                                      Number* g_scaling);

  /** @name Solution Methods */
  //@{
  /** This method is called after the optimization, and could write an
   *  output file with the optimal profiles */
  virtual void finalize_solution(SolverReturn status,
                                 Index n, const Number* x, const Number* z_L, const Number* z_U,
                                 Index m, const Number* g, const Number* lambda,
                                 Number obj_valu,
                                 const IpoptData* ip_data,
                                 IpoptCalculatedQuantities* ip_cq);
  //@}

protected:
  /** Method for setting the internal parameters that define the
   *  problem. It must be called by the child class in its
   *  implementation of InitializeParameters. */
  void SetBaseParameters(Index N, Number alpha, Number lb_y,
                         Number ub_y, Number lb_u, Number ub_u,
                         Number d_const, Number B, Number C);

  /**@name Functions that defines a particular instance. */
  //@{
  /** Target profile function for y */
  virtual Number y_d_cont(Number x1, Number x2, Number x3) const =0;
  //@}

private:
  /**@name Methods to block default compiler methods.
   * The compiler automatically generates the following three methods.
   *  Since the default compiler implementation is generally not what
   *  you want (for all but the most simple classes), we usually 
   *  put the declarations of these methods in the private section
   *  and never implement them. This prevents the compiler from
   *  implementing an incorrect "default" behavior without us
   *  knowing. (See Scott Meyers book, "Effective C++")
   *  
   */
  //@{
  MittelmannBndryCntrlDiriBase3D(const MittelmannBndryCntrlDiriBase3D&);
  MittelmannBndryCntrlDiriBase3D& operator=(const MittelmannBndryCntrlDiriBase3D&);
  //@}

  /**@name Problem specification */
  //@{
  /** Number of mesh points in one dimension (excluding boundary) */
  Index N_;
  /** Step size */
  Number h_;
  /** h_ squared */
  Number hh_;
  /** h_ to the third power */
  Number hhh_;
  /** overall lower bound on y */
  Number lb_y_;
  /** overall upper bound on y */
  Number ub_y_;
  /** overall lower bound on u */
  Number lb_u_;
  /** overall upper bound on u */
  Number ub_u_;
  /** Constant value of d appearing in elliptical equation */
  Number d_const_;
  /** Weighting parameter for the control target deviation functional
   *  in the objective */
  Number alpha_;
  /** Array for the target profile for y */
  Number* y_d_;
  //@}

  /**@name Auxilliary methods */
  //@{
  /** Translation of mesh point indices to NLP variable indices for
   *  y(x_ijk) */
  inline Index y_index(Index i, Index j, Index k) const
  {
    return k + (N_+2)*j + (N_+2)*(N_+2)*i;
  }
  /** Translation of interior mesh point indices to the corresponding
   *  PDE constraint number */
  inline Index pde_index(Index i, Index j, Index k) const
  {
    return (k-1) + N_*(j-1) + N_*N_*(i-1);
  }
  /** Compute the grid coordinate for given index in x1 direction */
  inline Number x1_grid(Index i) const
  {
    return h_*(Number)i;
  }
  /** Compute the grid coordinate for given index in x2 direction */
  inline Number x2_grid(Index i) const
  {
    return h_*(Number)i;
  }
  /** Compute the grid coordinate for given index in x3 direction */
  inline Number x3_grid(Index i) const
  {
    return h_*(Number)i;
  }
  /** value of penalty function term */
  inline Number PenObj(Number t) const
  {
    if (B_ == 0.) {
      return 0.5*t*t;
    }
    if (t > B_) {
      return B_*B_/2. + C_*(t - B_);
    }
    else if (t < -B_) {
      return B_*B_/2. + C_*(-t - B_);
    }
    else {
      const Number t2 = t*t;
      const Number t4 = t2*t2;
      const Number t6 = t4*t2;
      return PenA_*t2 + PenB_*t4 + PenC_*t6;
    }
  }
  /** first derivative of penalty function term */
  inline Number PenObj_1(Number t) const
  {
    if (B_ == 0.) {
      return t;
    }
    if (t > B_) {
      return C_;
    }
    else if (t < -B_) {
      return -C_;
    }
    else {
      const Number t2 = t*t;
      const Number t3 = t*t2;
      const Number t5 = t3*t2;
      return 2.*PenA_*t + 4.*PenB_*t3 + 6.*PenC_*t5;
    }
  }
  /** second derivative of penalty function term */
  inline Number PenObj_2(Number t) const
  {
    if (B_ == 0.) {
      return 1.;
    }
    if (t > B_) {
      return 0.;
    }
    else if (t < -B_) {
      return 0.;
    }
    else {
      const Number t2 = t*t;
      const Number t4 = t2*t2;
      return 2.*PenA_ + 12.*PenB_*t2 + 30.*PenC_*t4;
    }
  }
  //@}

  /** @name Data for penalty function term */
  //@{
  Number B_;
  Number C_;
  Number PenA_;
  Number PenB_;
  Number PenC_;
  //@}
};

/** Class implementating Example 1 */
class MittelmannBndryCntrlDiri3D_1 : public MittelmannBndryCntrlDiriBase3D
{
public:
  MittelmannBndryCntrlDiri3D_1()
  {}

  virtual ~MittelmannBndryCntrlDiri3D_1()
  {}

  virtual bool InitializeProblem(Index N)
  {
    if (N<1) {
      printf("N has to be at least 1.");
      return false;
    }
    Number alpha = 0.01;
    Number lb_y = -1e20;
    Number ub_y = 3.5;
    Number lb_u = 0.;
    Number ub_u = 10.;
    Number d_const = -20.;
    Number B = 0.;
    Number C = 0.;
    SetBaseParameters(N, alpha, lb_y, ub_y, lb_u, ub_u, d_const, B, C);
    return true;
  }
protected:
  /** Target profile function for y */
  virtual Number y_d_cont(Number x1, Number x2, Number x3)  const
  {
    //return 3. + 5.*(x1*(x1-1.)*x2*(x2-1.)); // change?
    return 2.8 + 40.*(x1*(x1-1.)*x2*(x2-1.)*x3*(x3-1.));
  }
private:
  /**@name hide implicitly defined contructors copy operators */
  //@{
  MittelmannBndryCntrlDiri3D_1(const MittelmannBndryCntrlDiri3D_1&);
  MittelmannBndryCntrlDiri3D_1& operator=(const MittelmannBndryCntrlDiri3D_1&);
  //@}

};


/** Class implementating Example 1 with nonconvex Beaton-Tukey like penalty
    function */
class MittelmannBndryCntrlDiri3D_1BT : public MittelmannBndryCntrlDiriBase3D
{
public:
  MittelmannBndryCntrlDiri3D_1BT()
  {}

  virtual ~MittelmannBndryCntrlDiri3D_1BT()
  {}

  virtual bool InitializeProblem(Index N)
  {
    if (N<1) {
      printf("N has to be at least 1.");
      return false;
    }
    Number alpha = 0.01;
    Number lb_y = -1e20;
    Number ub_y = 3.5;
    Number lb_u = 0.;
    Number ub_u = 10.;
    Number d_const = -20.;
    Number B = .75;
    Number C = 0.01;
    SetBaseParameters(N, alpha, lb_y, ub_y, lb_u, ub_u, d_const, B, C);
    return true;
  }
protected:
  /** Target profile function for y */
  virtual Number y_d_cont(Number x1, Number x2, Number x3)  const
  {
    //return 3. + 5.*(x1*(x1-1.)*x2*(x2-1.));
    return 2.8 + 40.*(x1*(x1-1.)*x2*(x2-1.)*x3*(x3-1.));
  }
private:
  /**@name hide implicitly defined contructors copy operators */
  //@{
  MittelmannBndryCntrlDiri3D_1BT(const MittelmannBndryCntrlDiri3D_1BT&);
  MittelmannBndryCntrlDiri3D_1BT& operator=(const MittelmannBndryCntrlDiri3D_1BT&);
  //@}
};

/** Class implementating Example 1 with nonconvex Beaton-Tukey like penalty
    function */
class MittelmannBndryCntrlDiri3D_1BTa : public MittelmannBndryCntrlDiriBase3D
{
public:
  MittelmannBndryCntrlDiri3D_1BTa()
  {}

  virtual ~MittelmannBndryCntrlDiri3D_1BTa()
  {}

  virtual bool InitializeProblem(Index N)
  {
    if (N<1) {
      printf("N has to be at least 1.");
      return false;
    }
    Number alpha = 0.01;
    Number lb_y = -1e20;
    Number ub_y = 3.5;
    Number lb_u = 0.;
    Number ub_u = 10.;
    Number d_const = -20.;
    Number B = .5;
    Number C = 0.01;
    SetBaseParameters(N, alpha, lb_y, ub_y, lb_u, ub_u, d_const, B, C);
    return true;
  }
protected:
  /** Target profile function for y */
  virtual Number y_d_cont(Number x1, Number x2, Number x3)  const
  {
    return 3. + 5.*(x1*(x1-1.)*x2*(x2-1.));
    //return 2.8 + 40.*(x1*(x1-1.)*x2*(x2-1.)*x3*(x3-1.));
  }
private:
  /**@name hide implicitly defined contructors copy operators */
  //@{
  MittelmannBndryCntrlDiri3D_1BTa(const MittelmannBndryCntrlDiri3D_1BTa&);
  MittelmannBndryCntrlDiri3D_1BTa& operator=(const MittelmannBndryCntrlDiri3D_1BTa&);
  //@}
};

/** Class implementating Example 2 */
class MittelmannBndryCntrlDiri3D_2 : public MittelmannBndryCntrlDiriBase3D
{
public:
  MittelmannBndryCntrlDiri3D_2()
  {}

  virtual ~MittelmannBndryCntrlDiri3D_2()
  {}

  virtual bool InitializeProblem(Index N)
  {
    if (N<1) {
      printf("N has to be at least 1.");
      return false;
    }
    Number alpha = 0.;
    Number lb_y = -1e20;
    Number ub_y = 3.5;
    Number lb_u = 0.;
    Number ub_u = 10.;
    Number d_const = -20.;
    Number B = 0.;
    Number C = 0.;
    SetBaseParameters(N, alpha, lb_y, ub_y, lb_u, ub_u, d_const, B, C);
    return true;
  }
protected:
  /** Target profile function for y */
  virtual Number y_d_cont(Number x1, Number x2, Number x3)  const
  {
    //return 3. + 5.*(x1*(x1-1.)*x2*(x2-1.)); // change?
    return 2.8 + 40.*(x1*(x1-1.)*x2*(x2-1.)*x3*(x3-1.));
  }
private:
  /**@name hide implicitly defined contructors copy operators */
  //@{
  MittelmannBndryCntrlDiri3D_2(const MittelmannBndryCntrlDiri3D_2&);
  MittelmannBndryCntrlDiri3D_2& operator=(const MittelmannBndryCntrlDiri3D_2&);
  //@}

};


/** Class implementating Example 1 with nonconvex Beaton-Tukey like penalty
    function */
class MittelmannBndryCntrlDiri3D_2BT : public MittelmannBndryCntrlDiriBase3D
{
public:
  MittelmannBndryCntrlDiri3D_2BT()
  {}

  virtual ~MittelmannBndryCntrlDiri3D_2BT()
  {}

  virtual bool InitializeProblem(Index N)
  {
    if (N<1) {
      printf("N has to be at least 1.");
      return false;
    }
    Number alpha = 0.;
    Number lb_y = -1e20;
    Number ub_y = 3.5;
    Number lb_u = 0.;
    Number ub_u = 10.;
    Number d_const = -20.;
    Number B = .75;
    Number C = 0.01;
    SetBaseParameters(N, alpha, lb_y, ub_y, lb_u, ub_u, d_const, B, C);
    return true;
  }
protected:
  /** Target profile function for y */
  virtual Number y_d_cont(Number x1, Number x2, Number x3)  const
  {
    //return 3. + 5.*(x1*(x1-1.)*x2*(x2-1.));
    return 2.8 + 40.*(x1*(x1-1.)*x2*(x2-1.)*x3*(x3-1.));
  }
private:
  /**@name hide implicitly defined contructors copy operators */
  //@{
  MittelmannBndryCntrlDiri3D_2BT(const MittelmannBndryCntrlDiri3D_2BT&);
  MittelmannBndryCntrlDiri3D_2BT& operator=(const MittelmannBndryCntrlDiri3D_2BT&);
  //@}
};

/** Class implementating Example 3 */
class MittelmannBndryCntrlDiri3D_3 : public MittelmannBndryCntrlDiriBase3D
{
public:
  MittelmannBndryCntrlDiri3D_3()
  {}

  virtual ~MittelmannBndryCntrlDiri3D_3()
  {}

  virtual bool InitializeProblem(Index N)
  {
    if (N<1) {
      printf("N has to be at least 1.");
      return false;
    }
    Number alpha = 0.01;
    Number lb_y = -1e20;
    Number ub_y = 3.2;
    Number lb_u = 1.6;
    Number ub_u = 2.3;
    Number d_const = -20.;
    Number B = 0.;
    Number C = 0.;
    SetBaseParameters(N, alpha, lb_y, ub_y, lb_u, ub_u, d_const, B, C);
    return true;
  }
protected:
  /** Target profile function for y */
  virtual Number y_d_cont(Number x1, Number x2, Number x3)  const
  {
    //    return 3. + 5.*(x1*(x1-1.)*x2*(x2-1.)); // change?
    return 2.8 + 40.*(x1*(x1-1.)*x2*(x2-1.)*x3*(x3-1.));
  }
private:
  /**@name hide implicitly defined contructors copy operators */
  //@{
  MittelmannBndryCntrlDiri3D_3(const MittelmannBndryCntrlDiri3D_3&);
  MittelmannBndryCntrlDiri3D_3& operator=(const MittelmannBndryCntrlDiri3D_3&);
  //@}

};


/** Class implementating Example 1 with nonconvex Beaton-Tukey like penalty
    function */
class MittelmannBndryCntrlDiri3D_3BT : public MittelmannBndryCntrlDiriBase3D
{
public:
  MittelmannBndryCntrlDiri3D_3BT()
  {}

  virtual ~MittelmannBndryCntrlDiri3D_3BT()
  {}

  virtual bool InitializeProblem(Index N)
  {
    if (N<1) {
      printf("N has to be at least 1.");
      return false;
    }
    Number alpha = 0.01;
    Number lb_y = -1e20;
    Number ub_y = 3.2;
    Number lb_u = 1.6;
    Number ub_u = 2.3;
    Number d_const = -20.;
    Number B = 0.75;
    Number C = 0.01;
    SetBaseParameters(N, alpha, lb_y, ub_y, lb_u, ub_u, d_const, B, C);
    return true;
  }
protected:
  /** Target profile function for y */
  virtual Number y_d_cont(Number x1, Number x2, Number x3)  const
  {
    //return 3. + 5.*(x1*(x1-1.)*x2*(x2-1.));
    return 2.8 + 40.*(x1*(x1-1.)*x2*(x2-1.)*x3*(x3-1.));
  }
private:
  /**@name hide implicitly defined contructors copy operators */
  //@{
  MittelmannBndryCntrlDiri3D_3BT(const MittelmannBndryCntrlDiri3D_3BT&);
  MittelmannBndryCntrlDiri3D_3BT& operator=(const MittelmannBndryCntrlDiri3D_3BT&);
  //@}
};

/** Class implementating Example 4 */
class MittelmannBndryCntrlDiri3D_4 : public MittelmannBndryCntrlDiriBase3D
{
public:
  MittelmannBndryCntrlDiri3D_4()
  {}

  virtual ~MittelmannBndryCntrlDiri3D_4()
  {}

  virtual bool InitializeProblem(Index N)
  {
    if (N<1) {
      printf("N has to be at least 1.");
      return false;
    }
    Number alpha = 0.;
    Number lb_y = -1e20;
    Number ub_y = 3.2;
    Number lb_u = 1.6;
    Number ub_u = 2.3;
    Number d_const = -20.;
    Number B = 0.;
    Number C = 0.;
    SetBaseParameters(N, alpha, lb_y, ub_y, lb_u, ub_u, d_const, B, C);
    return true;
  }
protected:
  /** Target profile function for y */
  virtual Number y_d_cont(Number x1, Number x2, Number x3)  const
  {
    //return 3. + 5.*(x1*(x1-1.)*x2*(x2-1.)); // change?
    return 2.8 + 40.*(x1*(x1-1.)*x2*(x2-1.)*x3*(x3-1.));
  }
private:
  /**@name hide implicitly defined contructors copy operators */
  //@{
  MittelmannBndryCntrlDiri3D_4(const MittelmannBndryCntrlDiri3D_4&);
  MittelmannBndryCntrlDiri3D_4& operator=(const MittelmannBndryCntrlDiri3D_4&);
  //@}

};


/** Class implementating Example 1 with nonconvex Beaton-Tukey like penalty
    function */
class MittelmannBndryCntrlDiri3D_4BT : public MittelmannBndryCntrlDiriBase3D
{
public:
  MittelmannBndryCntrlDiri3D_4BT()
  {}

  virtual ~MittelmannBndryCntrlDiri3D_4BT()
  {}

  virtual bool InitializeProblem(Index N)
  {
    if (N<1) {
      printf("N has to be at least 1.");
      return false;
    }
    Number alpha = 0.;
    Number lb_y = -1e20;
    Number ub_y = 3.2;
    Number lb_u = 1.6;
    Number ub_u = 2.3;
    Number d_const = -20.;
    Number B = .75;
    Number C = 0.01;
    SetBaseParameters(N, alpha, lb_y, ub_y, lb_u, ub_u, d_const, B, C);
    return true;
  }
protected:
  /** Target profile function for y */
  virtual Number y_d_cont(Number x1, Number x2, Number x3)  const
  {
    //return 3. + 5.*(x1*(x1-1.)*x2*(x2-1.));
    return 2.8 + 40.*(x1*(x1-1.)*x2*(x2-1.)*x3*(x3-1.));
  }
private:
  /**@name hide implicitly defined contructors copy operators */
  //@{
  MittelmannBndryCntrlDiri3D_4BT(const MittelmannBndryCntrlDiri3D_4BT&);
  MittelmannBndryCntrlDiri3D_4BT& operator=(const MittelmannBndryCntrlDiri3D_4BT&);
  //@}

};


#endif