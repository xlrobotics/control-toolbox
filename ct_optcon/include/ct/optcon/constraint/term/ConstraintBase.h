/***********************************************************************************
Copyright (c) 2017, Michael Neunert, Markus Giftthaler, Markus Stäuble, Diego Pardo,
Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of ETH ZURICH nor the names of its contributors may be used
      to endorse or promote products derived from this software without specific
      prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL ETH ZURICH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************************/

#ifndef CT_OPTCON_CONSTRAINT_TERM_CONSTRAINTBASE_H_
#define CT_OPTCON_CONSTRAINT_TERM_CONSTRAINTBASE_H_

//#include <ct/core/core.h>
//#include <Eigen/Sparse>
//#include <ct/core/internal/traits/TraitSelector.h>
//#include <ct/core/internal/traits/TraitSelectorSpecs.h>

namespace ct {
namespace optcon {

/**
 * @ingroup    Constraint
 *
 * @brief      Base class for the constraints used in this toolbox
 *
 * @tparam     STATE_DIM  The state dimension
 * @tparam     CONTROL_DIM  The control dimension
 * @tparam     SCALAR     The Scalar type
 */
template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR = double>
class ConstraintBase
{
public:

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	typedef typename ct::core::tpl::TraitSelector<SCALAR>::Trait Trait;

	typedef core::StateVector<STATE_DIM, SCALAR> state_vector_t;
	typedef core::ControlVector<CONTROL_DIM, SCALAR> control_vector_t;

	typedef Eigen::Matrix<SCALAR, Eigen::Dynamic, 1> VectorXs;
	typedef Eigen::Matrix<SCALAR, Eigen::Dynamic, Eigen::Dynamic> MatrixXs; 

	/**
	 * @brief      Custom constructor
	 *
	 * @param[in]  name  The name of the constraint
	 */
	ConstraintBase(std::string name = "Unnamed");

	/**
	 * @brief      Copy constructor
	 *
	 * @param[in]  arg   The object to be copied
	 */
	ConstraintBase(const ConstraintBase& arg);

	/**
	 * @brief      Creates a new instance of the object with same properties than original.
	 *
	 * @return     Copy of this object.
	 */
	virtual ConstraintBase<STATE_DIM, CONTROL_DIM, SCALAR>* clone () const = 0;

	/**
	 * @brief      Destructor
	 */
	virtual ~ConstraintBase();

	/**
	 * @brief      The evaluation of the constraint violation. Note this method
	 *             is SCALAR typed
	 *
	 * @param[in]  x     The state vector
	 * @param[in]  u     The control vector
	 * @param[in]  t     The time
	 *
	 * @return     The constraint violation
	 */
	virtual VectorXs evaluate(const state_vector_t& x, const control_vector_t& u, const SCALAR t) = 0;

	/**
	 * @brief      The evaluate method used for jit compilation in constraint
	 *             container ad
	 *
	 * @param[in]  x     The state vector
	 * @param[in]  u     The control vector
	 * @param[in]  t     The time
	 *
	 * @return     The constraint violation
	 */
	virtual Eigen::Matrix<ct::core::ADCGScalar, Eigen::Dynamic, 1> evaluateCppadCg(
		const core::StateVector<STATE_DIM, ct::core::ADCGScalar>& x, 
		const core::ControlVector<CONTROL_DIM, ct::core::ADCGScalar>& u,
		ct::core::ADCGScalar t);


	/**
	 * @brief      Returns the number of constraints
	 *
	 * @return     The number of constraints
	 */
	virtual size_t getConstraintSize() const = 0;


	/**
	 * @brief      Returns the constraint jacobian wrt state
	 *
	 * @return     The constraint jacobian
	 */
	virtual MatrixXs jacobianState(const state_vector_t& x, const control_vector_t& u, const SCALAR t);

	/**
	 * @brief      Returns the constraint jacobian wrt input
	 *
	 * @return     The constraint jacobian
	 */
	virtual MatrixXs jacobianInput(const state_vector_t& x, const control_vector_t& u, const SCALAR t);

	/**
	 * @brief      Returns the lower constraint bound
	 *
	 * @return     The lower constraint bound
	 */
	virtual VectorXs getLowerBound() const;

	/**
	 * @brief      Returns the upper constraint bound
	 *
	 * @return     The upper constraint bound
	 */
	virtual VectorXs getUpperBound() const;

	/**
	 * @brief      Returns the constraint name
	 *
	 * @param[out]      constraintName  The constraint name
	 */
	void getName(std::string& constraintName) const;

	/**
	 * @brief      Sets the constraint name.
	 *
	 * @param[in]  constraintName  The constraint name
	 */
	void setName(const std::string constraintName);

	/**
	 * @brief      Returns the number of nonzeros in the jacobian wrt state. The
	 *             default implementation assumes a dense matrix with only
	 *             nonzero elements.
	 *
	 * @return     The number of non zeros
	 */
	virtual size_t getNumNonZerosJacobianState() const;

	/**
	 * @brief      Returns the number of nonzeros in the jacobian wrt control
	 *             input. The default implementation assumes a dense matrix with
	 *             only nonzero elements
	 *
	 * @return     The number of non zeros
	 */
	virtual size_t getNumNonZerosJacobianInput() const;

	/**
	 * @brief      Returns the constraint jacobian wrt state in sparse
	 *             structure. The default implementation maps the JacobianState
	 *             matrix to a vector
	 *
	 * @return     The sparse constraint jacobian
	 */
	virtual VectorXs jacobianStateSparse(const state_vector_t& x, const control_vector_t& u, const SCALAR t);

	/**
	 * @brief      Returns the constraint jacobian wrt control input in sparse
	 *             structure. The default implementation maps the JacobianState
	 *             matrix to a vector
	 *
	 * @return     The sparse constraint jacobian
	 */
	virtual VectorXs jacobianInputSparse(const state_vector_t& x, const control_vector_t& u, const SCALAR t);


	/**
	 * @brief      Generates the sparsity pattern of the jacobian wrt state. The
	 *             default implementation returns a vector of ones corresponding
	 *             to the dense jacobianState
	 *
	 * @param      rows  The vector of the row indices containing non zero
	 *                   elements in the constraint jacobian
	 * @param      cols  The vector of the column indices containing non zero
	 *                   elements in the constraint jacobian
	 */
	virtual void sparsityPatternState(Eigen::VectorXi& rows, Eigen::VectorXi& cols);

	/**
	 * @brief      Generates the sparsity pattern of the jacobian wrt control
	 *             input. The default implementation returns a vector of ones
	 *             corresponding to the dense jacobianInput
	 *
	 * @param      rows  The vector of the row indices containing non zero
	 *                   elements in the constraint jacobian
	 * @param      cols  The vector of the column indices containing non zero
	 *                   elements in the constraint jacobian
	 */
	virtual void sparsityPatternInput(Eigen::VectorXi& rows, Eigen::VectorXi& cols);


protected:
	VectorXs lb_; // lower bound on the constraints
	VectorXs ub_; // upper bound on the constraints

	/**
	 * @brief      Generates indices of a diagonal square matrix
	 *
	 * @param[in]  num_elements  The number of elements
	 * @param[out] iRow_vec      The row vector
	 * @param[out] jCol_vec      The column vector
	 */
	void genDiagonalIndices(
			const size_t num_elements,
			Eigen::VectorXi& iRow_vec,
			Eigen::VectorXi& jCol_vec);

	/**
	 * @brief      Generates indices of a full matrix
	 *
	 * @param[in]  num_rows  The number of rows of the matrix
	 * @param[in]  num_cols  The number columns of the matrix
	 * @param[out] iRow_vec  The row vector
	 * @param[out] jCol_vec  The col vector
	 */
	void genBlockIndices(
			const size_t num_rows,
			const size_t num_cols,
			Eigen::VectorXi& iRow_vec,
			Eigen::VectorXi& jCol_vec);

private:
	std::string name_;

};

} // namespace optcon
} // namespace ct

#endif
