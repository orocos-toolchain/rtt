/***************************************************************************** 
 *  \author 
 *  	Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *		ORO_Geometry V0.2
 *
 *	\par History
 *		- $log$
 *
 *	\par Release
 *		$Id: utility_newmat.h,v 1.1.1.1.2.1 2003/01/06 16:14:02 psoetens Exp $
 *		$Name:  $ 
 *
 *  \file
 *  Included by  to provide some utility functions that connect
 *  ORO_Geometry objects to NEWMAT vectors and matrices.  Also provides some
 *  I/O of NEWMAT objects.
 */
#ifndef UTILITY_NEWMAT_H
#define UTILITY_NEWMAT_H

#include "utility.h"
#include "utility_io.h"
#include <newmat/newmat.h>
#include <string>
#include <vector>


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


/**
 * Takes the adjoint of a diagonal matrix :
 * @param v : input diagonal matrix
 * @param r : output diagonal matrix
 */
void Adj(const NEWMAT::DiagonalMatrix& v,NEWMAT::DiagonalMatrix& r);



/** Takes the sum over all the elements of the elementwise 
 * product of A-transpose and B
 */
double SumOfPrd(const NEWMAT::Matrix& A,const NEWMAT::Matrix& B);




//! Type that stores a vector of Matrices.
typedef std::vector<NEWMAT::Matrix> MatrixVector;

//! Initialize a vector of matrices :
inline void InitMatrixVector(MatrixVector& v,int n,int m) {
    v.resize(VSIZE);
    for (MatrixVector::iterator it=v.begin();it!=v.end();++it) {
        *it = NEWMAT::Matrix(n,m);
    }
}

/** defines equality between matrices with a tolerance
 * @param eps tollerance that is used when comparing the matrices.
 * @return true if matrices are equal otherwise false.
 */
bool Equal(const NEWMAT::Matrix& a,const NEWMAT::Matrix& b,double eps=epsilon);


/** Reads a named diagonal matrix in from a stream
 * the stream should contain something like : 
 *   matrixname[el1, el2, ... ]
 * @param name  the name of the item read from the stream (matrixname in the example)
 * @param is    inputstream
 * @param vect  will contain the diagonalmatrix when the function returns
 * @param size  number of elements of the diagonalmatrix
 */
void ReadNamedDiagMatrix(const char*name,istream& is,NEWMAT::DiagonalMatrix& vect,int size);



/** Writes a named diagonal matrix to a stream
 * the stream will contain something like : 
 *   matrixname[el1, el2, ... ]
 * @param name  the name of the item read from the stream (matrixname in the example)
 * @param os    outputstream
 * @param vect  contains the diagonalmatrix to be written
 */
void WriteNamedDiagMatrix(const char* name,ostream& os,const NEWMAT::DiagonalMatrix& vect);



/** Reads a named vector in from a stream
 * the stream should contain something like : 
 *   vectorname[el1, el2, ... ]
 * @param name  the name of the item read from the stream (vectorname in the example)
 * @param is    inputstream
 * @param vect  will contain the vectormatrix when the function returns
 * @param size  number of elements of the diagonalmatrix
 */
void ReadNamedVector(const char*name,istream& is,NEWMAT::ColumnVector& vect,int size);



/** Writes a named vector to a stream
 * the stream will contain something like : 
 *   vectorname[el1, el2, ... ]
 * @param name  the name of the item read from the stream (vectorname in the example)
 * @param os    outputstream
 * @param vect  contains the vector to be written
 */
void WriteNamedVector(const char* name,ostream& os,const NEWMAT::ColumnVector& vect);





/** Reads a named string in from a stream
 * the stream should contain something like : 
 *   stringname[string that contains the desired information]
 * @param name  the name of the item read from the stream (stringname in the example)
 * @param is    inputstream
 * @param str   A STL-string that will contain the string when the function returns.
 */
void ReadNamedString(const char*name,istream& is,std::string& str);



/** Writes a named string to a stream
 * the stream will contain something like : 
 *   stringname[el1, el2, ... ]
 * @param name  the name of the item read from the stream (stringname in the example)
 * @param os    outputstream
 * @param str   contains the STL-string to be written
 */
void WriteNamedString(const char* name,ostream& os,const std::string& str);



/** Reads a named value in from a stream
 * the stream should contain something like : 
 *   valuename[string that contains the desired information]
 * @param name  the name of the item read from the stream (matrixname in the example)
 * @param is    inputstream
 * @param value A double that will contain the string when the function returns.
 */
void ReadNamedValue(const char*name,istream& is,double& value);



/** Writes a named string to a stream
 * the stream will contain something like : 
 *   valuename[el1, el2, ... ]
 * @param name  the name of the item read from the stream (valuename in the example)
 * @param os    outputstream
 * @param value contains the value to be written
 */
void WriteNamedValue(const char* name,ostream& os,const double value);


/**
 * pseudo-inverse of a diagonal matrix.
 * @param D (in/out ) Diagonal matrix
 */
void PInvDiagonalMatrix(NEWMAT::DiagonalMatrix& D);





#ifdef USE_NAMESPACE
}
#endif



#endif
