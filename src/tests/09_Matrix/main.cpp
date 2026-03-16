#include <UCommon/Matrix.h>
#include <UCommon/Vector.h>

#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>

using namespace UCommon;

// Helper function to compare floats with tolerance
bool AlmostEqual(float a, float b, float epsilon = 1e-5f)
{
	return std::abs(a - b) < epsilon;
}

// Helper function to compare vectors with tolerance
bool AlmostEqual(const FVector3f& a, const FVector3f& b, float epsilon = 1e-5f)
{
	return AlmostEqual(a.X, b.X, epsilon) &&
	       AlmostEqual(a.Y, b.Y, epsilon) &&
	       AlmostEqual(a.Z, b.Z, epsilon);
}

void PrintMatrix3x3(const FMatrix3x3f& M)
{
	std::cout << std::fixed << std::setprecision(3);
	for (int i = 0; i < 3; ++i)
	{
		std::cout << "  | " << M(i, 0) << " " << M(i, 1) << " " << M(i, 2) << " |" << std::endl;
	}
}

void PrintMatrix4x4(const FMatrix4x4f& M)
{
	std::cout << std::fixed << std::setprecision(3);
	for (int i = 0; i < 4; ++i)
	{
		std::cout << "  | " << M(i, 0) << " " << M(i, 1) << " " << M(i, 2) << " " << M(i, 3) << " |" << std::endl;
	}
}

void TestMatrix3x3_Constructors()
{
	std::cout << "=== Testing TMatrix3x3 Constructors ===" << std::endl;

	// Default constructor (uninitialized)
	{
		FMatrix3x3f M;
		std::cout << "  Default constructor: [PASSED]" << std::endl;
	}

	// Value constructor
	{
		FMatrix3x3f M(2.0f);
		assert(M(0, 0) == 2.0f && M(1, 1) == 2.0f && M(2, 2) == 2.0f);
		std::cout << "  Value constructor: [PASSED]" << std::endl;
	}

	// Element-wise constructor
	{
		FMatrix3x3f M(
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);
		assert(M(0, 0) == 1 && M(0, 1) == 2 && M(0, 2) == 3);
		assert(M(1, 0) == 4 && M(1, 1) == 5 && M(1, 2) == 6);
		assert(M(2, 0) == 7 && M(2, 1) == 8 && M(2, 2) == 9);
		std::cout << "  Element-wise constructor: [PASSED]" << std::endl;
	}

	// Row vector constructor
	{
		FMatrix3x3f M(
			FVector3f(1, 2, 3),
			FVector3f(4, 5, 6),
			FVector3f(7, 8, 9)
		);
		assert(M[0].X == 1 && M[0].Y == 2 && M[0].Z == 3);
		assert(M[1].X == 4 && M[1].Y == 5 && M[1].Z == 6);
		assert(M[2].X == 7 && M[2].Y == 8 && M[2].Z == 9);
		std::cout << "  Row vector constructor: [PASSED]" << std::endl;
	}

	// Identity matrix
	{
		FMatrix3x3f M = FMatrix3x3f::Identity();
		assert(M(0, 0) == 1 && M(1, 1) == 1 && M(2, 2) == 1);
		assert(M(0, 1) == 0 && M(0, 2) == 0);
		assert(M(1, 0) == 0 && M(1, 2) == 0);
		assert(M(2, 0) == 0 && M(2, 1) == 0);
		std::cout << "  Identity matrix: [PASSED]" << std::endl;
	}

	// Zero matrix
	{
		FMatrix3x3f M = FMatrix3x3f::Zero();
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				assert(M(i, j) == 0.0f);
			}
		}
		std::cout << "  Zero matrix: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix3x3_VectorMultiplication()
{
	std::cout << "=== Testing TMatrix3x3 Vector Multiplication ===" << std::endl;

	// Identity * vector = vector
	{
		FMatrix3x3f I = FMatrix3x3f::Identity();
		FVector3f v(1, 2, 3);
		FVector3f result = I * v;

		assert(AlmostEqual(result, v));
		std::cout << "  Identity * vector: [PASSED]" << std::endl;
	}

	// Scaling matrix
	{
		FMatrix3x3f S = FMatrix3x3f::Scaling(FVector3f(2.0f, 3.0f, 4.0f));
		FVector3f v(1, 1, 1);
		FVector3f result = S * v;

		assert(AlmostEqual(result, FVector3f(2, 3, 4)));
		std::cout << "  Scaling matrix * vector: [PASSED]" << std::endl;
	}

	// 2D point transformation (with w=1)
	{
		FMatrix3x3f M(
			2, 0, 5,
			0, 3, 7,
			0, 0, 1
		);
		FVector2f p(1, 2);
		FVector2f result = M * p;

		// Result should be: (2*1+5, 3*2+7) = (7, 13)
		assert(AlmostEqual(result.X, 7.0f));
		assert(AlmostEqual(result.Y, 13.0f));
		std::cout << "  2D point transformation: [PASSED]" << std::endl;
	}

	// 2D vector transformation (with w=0)
	{
		FMatrix3x3f M(
			2, 0, 5,
			0, 3, 7,
			0, 0, 1
		);
		FVector2f v(1, 2);
		FVector2f result = M.TransformVector(v);

		// Translation should not affect vectors: (2*1, 3*2) = (2, 6)
		assert(AlmostEqual(result.X, 2.0f));
		assert(AlmostEqual(result.Y, 6.0f));
		std::cout << "  2D vector transformation: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix3x3_Rotation()
{
	std::cout << "=== Testing TMatrix3x3 Rotation ===" << std::endl;

	// Rotation around X axis (90 degrees = pi/2)
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix3x3f M = FMatrix3x3f::RotationX(angle);
		FVector3f v(0, 1, 0);
		FVector3f result = M * v;

		// Point on Y axis should rotate to Z axis
		assert(AlmostEqual(result.X, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Z, 1.0f, 1e-4f));
		std::cout << "  Rotation X (90°): [PASSED]" << std::endl;
	}

	// Rotation around Y axis (90 degrees)
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix3x3f M = FMatrix3x3f::RotationY(angle);
		FVector3f v(1, 0, 0);
		FVector3f result = M * v;

		// Point on X axis should rotate to -Z axis
		assert(AlmostEqual(result.X, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Z, -1.0f, 1e-4f));
		std::cout << "  Rotation Y (90°): [PASSED]" << std::endl;
	}

	// Rotation around Z axis (90 degrees)
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix3x3f M = FMatrix3x3f::RotationZ(angle);
		FVector3f v(1, 0, 0);
		FVector3f result = M * v;

		// Point on X axis should rotate to Y axis
		assert(AlmostEqual(result.X, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 1.0f, 1e-4f));
		assert(AlmostEqual(result.Z, 0.0f, 1e-4f));
		std::cout << "  Rotation Z (90°): [PASSED]" << std::endl;
	}

	// Axis-angle rotation (rotate (1,0,0) around (0,1,0) by 90°)
	{
		float angle = 3.14159265f / 2.0f;
		FVector3f axis(0, 1, 0);
		FMatrix3x3f M = FMatrix3x3f::Rotation(axis, angle);
		FVector3f v(1, 0, 0);
		FVector3f result = M * v;

		// Should be (0, 0, -1)
		assert(AlmostEqual(result.X, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Z, -1.0f, 1e-4f));
		std::cout << "  Axis-angle rotation: [PASSED]" << std::endl;
	}

	// 360 degree rotation should return identity
	{
		float angle = 2.0f * 3.14159265f;
		FVector3f axis(1, 1, 1);
		FMatrix3x3f M = FMatrix3x3f::Rotation(axis.SafeNormalize(), angle);
		FVector3f v(1, 2, 3);
		FVector3f result = M * v;

		assert(AlmostEqual(result, v, 1e-3f));
		std::cout << "  360° rotation (identity): [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix3x3_Operations()
{
	std::cout << "=== Testing TMatrix3x3 Operations ===" << std::endl;

	// Transpose
	{
		FMatrix3x3f M(
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);
		FMatrix3x3f MT = M.Transpose();

		assert(MT(0, 0) == M(0, 0) && MT(1, 1) == M(1, 1) && MT(2, 2) == M(2, 2));
		assert(MT(0, 1) == M(1, 0) && MT(0, 2) == M(2, 0));
		assert(MT(1, 0) == M(0, 1) && MT(1, 2) == M(2, 1));
		assert(MT(2, 0) == M(0, 2) && MT(2, 1) == M(1, 2));
		std::cout << "  Transpose: [PASSED]" << std::endl;
	}

	// Matrix-Matrix multiplication
	{
		FMatrix3x3f A(
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);
		FMatrix3x3f B = FMatrix3x3f::Identity();
		FMatrix3x3f C = A * B;

		assert(C(0, 0) == A(0, 0) && C(1, 1) == A(1, 1) && C(2, 2) == A(2, 2));
		std::cout << "  Matrix * Identity: [PASSED]" << std::endl;
	}

	// Determinant of identity
	{
		FMatrix3x3f I = FMatrix3x3f::Identity();
		float det = I.Determinant();

		assert(AlmostEqual(det, 1.0f));
		std::cout << "  Determinant (Identity): [PASSED]" << std::endl;
	}

	// Trace
	{
		FMatrix3x3f M(
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);
		float trace = M.Trace();

		assert(AlmostEqual(trace, 15.0f)); // 1 + 5 + 9
		std::cout << "  Trace: [PASSED]" << std::endl;
	}

	// Inverse of identity is identity
	{
		FMatrix3x3f I = FMatrix3x3f::Identity();
		FMatrix3x3f IInv = I.Inverse();

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				AlmostEqual(I(i, j), IInv(i, j), 1e-5f);
			}
		}
		std::cout << "  Inverse of Identity: [PASSED]" << std::endl;
	}

	// Inverse * Matrix = Identity
	{
		FMatrix3x3f M(
			2, 0, 0,
			0, 3, 0,
			0, 0, 4
		);
		FMatrix3x3f MInv = M.Inverse();
		FMatrix3x3f Result = MInv * M;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				float expected = (i == j) ? 1.0f : 0.0f;
				assert(AlmostEqual(Result(i, j), expected, 1e-4f));
			}
		}
		std::cout << "  Inverse * Matrix = Identity: [PASSED]" << std::endl;
	}

	// Matrix comparison
	{
		FMatrix3x3f A = FMatrix3x3f::Identity();
		FMatrix3x3f B = FMatrix3x3f::Identity();
		FMatrix3x3f C = FMatrix3x3f::Zero();

		assert(A == B);
		assert(A != C);
		std::cout << "  Matrix comparison: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix4x4_Constructors()
{
	std::cout << "=== Testing TMatrix4x4 Constructors ===" << std::endl;

	// Default constructor
	{
		FMatrix4x4f M;
		std::cout << "  Default constructor: [PASSED]" << std::endl;
	}

	// Value constructor
	{
		FMatrix4x4f M(3.0f);
		assert(M(0, 0) == 3.0f && M(1, 1) == 3.0f && M(2, 2) == 3.0f && M(3, 3) == 3.0f);
		std::cout << "  Value constructor: [PASSED]" << std::endl;
	}

	// Element-wise constructor
	{
		FMatrix4x4f M(
			1, 2, 3, 4,
			5, 6, 7, 8,
			9, 10, 11, 12,
			13, 14, 15, 16
		);
		assert(M(0, 0) == 1 && M(0, 3) == 4);
		assert(M(1, 0) == 5 && M(1, 3) == 8);
		assert(M(2, 0) == 9 && M(2, 3) == 12);
		assert(M(3, 0) == 13 && M(3, 3) == 16);
		std::cout << "  Element-wise constructor: [PASSED]" << std::endl;
	}

	// Row vector constructor
	{
		FMatrix4x4f M(
			FVector4f(1, 2, 3, 4),
			FVector4f(5, 6, 7, 8),
			FVector4f(9, 10, 11, 12),
			FVector4f(13, 14, 15, 16)
		);
		assert(M[0].X == 1 && M[0].W == 4);
		assert(M[1].X == 5 && M[1].W == 8);
		assert(M[2].X == 9 && M[2].W == 12);
		assert(M[3].X == 13 && M[3].W == 16);
		std::cout << "  Row vector constructor: [PASSED]" << std::endl;
	}

	// Identity and Zero
	{
		FMatrix4x4f I = FMatrix4x4f::Identity();
		FMatrix4x4f Z = FMatrix4x4f::Zero();
		assert(I(0, 0) == 1 && I(1, 1) == 1 && I(2, 2) == 1 && I(3, 3) == 1);
		assert(Z(0, 0) == 0 && Z(3, 3) == 0);
		std::cout << "  Identity and Zero: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix4x4_Transformations()
{
	std::cout << "=== Testing TMatrix4x4 Transformations ===" << std::endl;

	// Translation
	{
		FVector3f t(5, 10, 15);
		FMatrix4x4f T = FMatrix4x4f::Translation(t);
		FVector3f p(1, 2, 3);
		FVector3f result = T * p;

		assert(AlmostEqual(result.X, 6.0f));
		assert(AlmostEqual(result.Y, 12.0f));
		assert(AlmostEqual(result.Z, 18.0f));
		std::cout << "  Translation: [PASSED]" << std::endl;
	}

	// Scaling
	{
		FMatrix4x4f S = FMatrix4x4f::Scaling(FVector3f(2, 3, 4));
		FVector3f p(1, 1, 1);
		FVector3f result = S * p;

		assert(AlmostEqual(result, FVector3f(2, 3, 4)));
		std::cout << "  Scaling: [PASSED]" << std::endl;
	}

	// Rotation around X axis
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix4x4f R = FMatrix4x4f::RotationX(angle);
		FVector3f p(0, 1, 0);
		FVector3f result = R * p;

		assert(AlmostEqual(result.X, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Z, 1.0f, 1e-4f));
		std::cout << "  Rotation X (4x4): [PASSED]" << std::endl;
	}

	// Rotation around Y axis
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix4x4f R = FMatrix4x4f::RotationY(angle);
		FVector3f p(1, 0, 0);
		FVector3f result = R * p;

		assert(AlmostEqual(result.X, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Z, -1.0f, 1e-4f));
		std::cout << "  Rotation Y (4x4): [PASSED]" << std::endl;
	}

	// Rotation around Z axis
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix4x4f R = FMatrix4x4f::RotationZ(angle);
		FVector3f p(1, 0, 0);
		FVector3f result = R * p;

		assert(AlmostEqual(result.X, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 1.0f, 1e-4f));
		assert(AlmostEqual(result.Z, 0.0f, 1e-4f));
		std::cout << "  Rotation Z (4x4): [PASSED]" << std::endl;
	}

	// Axis-angle rotation (4x4)
	{
		float angle = 3.14159265f / 2.0f;
		FVector3f axis(0, 1, 0);
		FMatrix4x4f R = FMatrix4x4f::Rotation(axis, angle);
		FVector3f p(1, 0, 0);
		FVector3f result = R * p;

		assert(AlmostEqual(result.X, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Z, -1.0f, 1e-4f));
		std::cout << "  Axis-angle rotation (4x4): [PASSED]" << std::endl;
	}

	// Vector transformation (w=0, no translation)
	{
		FMatrix4x4f T = FMatrix4x4f::Translation(FVector3f(10, 20, 30));
		FVector3f v(1, 0, 0);
		FVector3f result = T.TransformVector(v);

		assert(AlmostEqual(result.X, 1.0f));
		assert(AlmostEqual(result.Y, 0.0f));
		assert(AlmostEqual(result.Z, 0.0f));
		std::cout << "  Vector transformation (no translation): [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix4x4_Trs()
{
	std::cout << "=== Testing TMatrix4x4 TRS ===" << std::endl;

	// TRS combination
	{
		FVector3f translation(5, 10, 15);
		FVector3f rotationAxis(0, 1, 0);
		float rotationAngle = 0.0f; // No rotation
		FVector3f scale(2, 3, 4);

		FMatrix4x4f M = FMatrix4x4f::TRS(translation, rotationAxis, rotationAngle, scale);
		FVector3f p(1, 2, 3);
		FVector3f result = M * p;

		// (1*2+5, 2*3+10, 3*4+15) = (7, 16, 27)
		assert(AlmostEqual(result.X, 7.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 16.0f, 1e-4f));
		assert(AlmostEqual(result.Z, 27.0f, 1e-4f));
		std::cout << "  TRS (no rotation): [PASSED]" << std::endl;
	}

	// TRS with rotation
	{
		FVector3f translation(0, 0, 0);
		FVector3f rotationAxis(0, 1, 0);
		float rotationAngle = 3.14159265f / 2.0f; // 90° around Y
		FVector3f scale(1, 1, 1);

		FMatrix4x4f M = FMatrix4x4f::TRS(translation, rotationAxis, rotationAngle, scale);
		FVector3f p(1, 0, 0);
		FVector3f result = M * p;

		assert(AlmostEqual(result.X, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Z, -1.0f, 1e-4f));
		std::cout << "  TRS with rotation: [PASSED]" << std::endl;
	}

	// Extract components
	{
		FVector3f translation(1, 2, 3);
		FVector3f rotationAxis(0, 0, 1);
		float rotationAngle = 0.0f;
		FVector3f scale(2, 3, 4);

		FMatrix4x4f M = FMatrix4x4f::TRS(translation, rotationAxis, rotationAngle, scale);

		FVector3f extractedTrans = M.GetTranslation();
		FVector3f extractedScale = M.GetScale();

		assert(AlmostEqual(extractedTrans.X, 1.0f, 1e-4f));
		assert(AlmostEqual(extractedTrans.Y, 2.0f, 1e-4f));
		assert(AlmostEqual(extractedTrans.Z, 3.0f, 1e-4f));

		assert(AlmostEqual(extractedScale.X, 2.0f, 1e-4f));
		assert(AlmostEqual(extractedScale.Y, 3.0f, 1e-4f));
		assert(AlmostEqual(extractedScale.Z, 4.0f, 1e-4f));
		std::cout << "  Extract TRS components: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix4x4_Vector4()
{
	std::cout << "=== Testing TMatrix4x4 with TVector4 ===" << std::endl;

	// Transform TVector4
	{
		FMatrix4x4f T = FMatrix4x4f::Translation(FVector3f(5, 10, 15));
		FVector4f v(1, 2, 3, 1);  // w=1 means point
		FVector4f result = T * v;

		assert(AlmostEqual(result.X, 6.0f));
		assert(AlmostEqual(result.Y, 12.0f));
		assert(AlmostEqual(result.Z, 18.0f));
		assert(AlmostEqual(result.W, 1.0f));
		std::cout << "  Transform TVector4 (point): [PASSED]" << std::endl;
	}

	// Transform TVector4 with w=0 (vector)
	{
		FMatrix4x4f T = FMatrix4x4f::Translation(FVector3f(5, 10, 15));
		FVector4f v(1, 2, 3, 0);  // w=0 means vector
		FVector4f result = T * v;

		// Translation should not affect
		assert(AlmostEqual(result.X, 1.0f));
		assert(AlmostEqual(result.Y, 2.0f));
		assert(AlmostEqual(result.Z, 3.0f));
		assert(AlmostEqual(result.W, 0.0f));
		std::cout << "  Transform TVector4 (vector): [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix4x4_Inverse()
{
	std::cout << "=== Testing TMatrix4x4 Inverse ===" << std::endl;

	// Inverse of identity is identity
	{
		FMatrix4x4f I = FMatrix4x4f::Identity();
		FMatrix4x4f IInv = I.Inverse();

		assert(I(0, 0) == IInv(0, 0) && I(3, 3) == IInv(3, 3));
		std::cout << "  Inverse of Identity: [PASSED]" << std::endl;
	}

	// Translation inverse
	{
		FMatrix4x4f T = FMatrix4x4f::Translation(FVector3f(5, 10, 15));
		FMatrix4x4f TInv = T.Inverse();
		FMatrix4x4f Result = T * TInv;

		FMatrix4x4f I = FMatrix4x4f::Identity();
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				assert(AlmostEqual(Result(i, j), I(i, j), 1e-4f));
			}
		}
		std::cout << "  Translation inverse: [PASSED]" << std::endl;
	}

	// Scaling inverse
	{
		FMatrix4x4f S = FMatrix4x4f::Scaling(FVector3f(2, 3, 4));
		FMatrix4x4f SInv = S.Inverse();

		FMatrix4x4f Result = SInv * S;

		FMatrix4x4f I = FMatrix4x4f::Identity();
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				assert(AlmostEqual(Result(i, j), I(i, j), 1e-4f));
			}
		}
		std::cout << "  Scaling inverse: [PASSED]" << std::endl;
	}

	// Rotation inverse (rotation matrices are orthogonal)
	{
		float angle = 3.14159265f / 4.0f; // 45 degrees
		FVector3f axis(1, 1, 1);
		FMatrix4x4f R = FMatrix4x4f::Rotation(axis.SafeNormalize(), angle);
		FMatrix4x4f RInv = R.Inverse();

		FMatrix4x4f Result = RInv * R;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				float expected = (i == j) ? 1.0f : 0.0f;
				assert(AlmostEqual(Result(i, j), expected, 1e-3f));
			}
		}
		std::cout << "  Rotation inverse: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix3x3_UniformScaling()
{
	std::cout << "=== Testing TMatrix3x3 Uniform Scaling ===" << std::endl;

	// Uniform scaling using scalar parameter
	{
		float uniformScale = 3.0f;
		FMatrix3x3f S = FMatrix3x3f::Scaling(uniformScale);
		FVector3f v(1, 2, 3);
		FVector3f result = S * v;

		assert(AlmostEqual(result.X, 3.0f));
		assert(AlmostEqual(result.Y, 6.0f));
		assert(AlmostEqual(result.Z, 9.0f));
		std::cout << "  Uniform scaling (scalar): [PASSED]" << std::endl;
	}

	// Verify uniform scaling equals vector scaling
	{
		float uniformScale = 2.5f;
		FMatrix3x3f S1 = FMatrix3x3f::Scaling(uniformScale);
		FMatrix3x3f S2 = FMatrix3x3f::Scaling(FVector3f(uniformScale));

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				assert(AlmostEqual(S1(i, j), S2(i, j)));
			}
		}
		std::cout << "  Uniform scaling vs vector scaling: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix3x3_MatrixMultiplication()
{
	std::cout << "=== Testing TMatrix3x3 Matrix Multiplication ===" << std::endl;

	// Two general matrices multiplication
	{
		FMatrix3x3f A(
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);
		FMatrix3x3f B(
			9, 8, 7,
			6, 5, 4,
			3, 2, 1
		);
		FMatrix3x3f C = A * B;

		// Calculate expected: first row of result
		// C(0,0) = 1*9 + 2*6 + 3*3 = 9 + 12 + 9 = 30
		// C(0,1) = 1*8 + 2*5 + 3*2 = 8 + 10 + 6 = 24
		// C(0,2) = 1*7 + 2*4 + 3*1 = 7 + 8 + 3 = 18
		assert(AlmostEqual(C(0, 0), 30.0f));
		assert(AlmostEqual(C(0, 1), 24.0f));
		assert(AlmostEqual(C(0, 2), 18.0f));

		// C(1,0) = 4*9 + 5*6 + 6*3 = 36 + 30 + 18 = 84
		assert(AlmostEqual(C(1, 0), 84.0f));
		std::cout << "  Matrix * Matrix: [PASSED]" << std::endl;
	}

	// Non-commutative property check
	{
		FMatrix3x3f A = FMatrix3x3f::RotationX(0.5f);
		FMatrix3x3f B = FMatrix3x3f::RotationY(0.3f);
		FMatrix3x3f AB = A * B;
		FMatrix3x3f BA = B * A;

		// Rotation matrices generally don't commute
		bool isEqual = true;
		for (int i = 0; i < 3 && isEqual; ++i)
		{
			for (int j = 0; j < 3 && isEqual; ++j)
			{
				if (!AlmostEqual(AB(i, j), BA(i, j)))
					isEqual = false;
			}
		}
		assert(!isEqual);
		std::cout << "  Non-commutative property: [PASSED]" << std::endl;
	}

	// A * A^(-1) = Identity
	{
		FMatrix3x3f M(
			2, 0, 0,
			0, 3, 0,
			0, 0, 4
		);
		FMatrix3x3f MInv = M.Inverse();
		FMatrix3x3f Result = M * MInv;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				float expected = (i == j) ? 1.0f : 0.0f;
				assert(AlmostEqual(Result(i, j), expected, 1e-4f));
			}
		}
		std::cout << "  M * M^(-1) = Identity: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix3x3_AdditionalOperations()
{
	std::cout << "=== Testing TMatrix3x3 Additional Operations ===" << std::endl;

	// Determinant of diagonal matrix
	{
		FMatrix3x3f M(
			2, 0, 0,
			0, 3, 0,
			0, 0, 4
		);
		float det = M.Determinant();
		assert(AlmostEqual(det, 24.0f)); // 2 * 3 * 4
		std::cout << "  Determinant of diagonal matrix: [PASSED]" << std::endl;
	}

	// Determinant of non-identity matrix
	{
		FMatrix3x3f M(
			1, 2, 3,
			0, 1, 4,
			5, 6, 0
		);
		float det = M.Determinant();
		// det = 1*(1*0 - 4*6) - 2*(0*0 - 4*5) + 3*(0*6 - 1*5)
		//     = 1*(-24) - 2*(-20) + 3*(-5)
		//     = -24 + 40 - 15 = 1
		assert(AlmostEqual(det, 1.0f));
		std::cout << "  Determinant of non-identity: [PASSED]" << std::endl;
	}

	// Inverse of scaling matrix
	{
		FMatrix3x3f S = FMatrix3x3f::Scaling(FVector3f(2, 3, 4));
		FMatrix3x3f SInv = S.Inverse();

		// Check diagonal values
		assert(AlmostEqual(SInv(0, 0), 0.5f));
		assert(AlmostEqual(SInv(1, 1), 1.0f / 3.0f, 1e-5f));
		assert(AlmostEqual(SInv(2, 2), 0.25f));
		std::cout << "  Inverse of scaling: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix4x4_Operations()
{
	std::cout << "=== Testing TMatrix4x4 Operations ===" << std::endl;

	// Transpose
	{
		FMatrix4x4f M(
			1, 2, 3, 4,
			5, 6, 7, 8,
			9, 10, 11, 12,
			13, 14, 15, 16
		);
		FMatrix4x4f MT = M.Transpose();

		assert(MT(0, 0) == 1 && MT(3, 3) == 16);
		assert(MT(0, 3) == 13 && MT(3, 0) == 4);
		assert(MT(1, 0) == 2 && MT(0, 1) == 5);
		std::cout << "  Transpose: [PASSED]" << std::endl;
	}

	// Determinant of diagonal 4x4
	{
		FMatrix4x4f M(
			2, 0, 0, 0,
			0, 3, 0, 0,
			0, 0, 4, 0,
			0, 0, 0, 1
		);
		float det = M.Determinant();
		assert(AlmostEqual(det, 24.0f)); // 2 * 3 * 4 * 1
		std::cout << "  Determinant (diagonal): [PASSED]" << std::endl;
	}

	// Trace
	{
		FMatrix4x4f M(
			1, 2, 3, 4,
			5, 6, 7, 8,
			9, 10, 11, 12,
			13, 14, 15, 16
		);
		float trace = M.Trace();

		// 1 + 6 + 11 + 16 = 34
		assert(AlmostEqual(trace, 34.0f));
		std::cout << "  Trace: [PASSED]" << std::endl;
	}

	// Matrix comparison
	{
		FMatrix4x4f A = FMatrix4x4f::Identity();
		FMatrix4x4f B = FMatrix4x4f::Identity();
		FMatrix4x4f C = FMatrix4x4f::Zero();

		assert(A == B);
		assert(A != C);
		std::cout << "  Matrix comparison: [PASSED]" << std::endl;
	}

	// Matrix-Matrix multiplication
	{
		FMatrix4x4f T = FMatrix4x4f::Translation(FVector3f(1, 2, 3));
		FMatrix4x4f S = FMatrix4x4f::Scaling(FVector3f(2, 3, 4));
		FMatrix4x4f M = S * T; // Apply translation first, then scale

		FVector3f p(1, 0, 0);
		FVector3f result = M * p;

		// First translate: (1+1, 0+2, 0+3) = (2, 2, 3)
		// Then scale: (2*2, 2*3, 3*4) = (4, 6, 12)
		assert(AlmostEqual(result.X, 4.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 6.0f, 1e-4f));
		assert(AlmostEqual(result.Z, 12.0f, 1e-4f));
		std::cout << "  Matrix * Matrix: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix4x4_SetMethods()
{
	std::cout << "=== Testing TMatrix4x4 Set Methods ===" << std::endl;

	// SetRotation on existing matrix
	{
		FMatrix4x4f M = FMatrix4x4f::Zero();
		float angle = 3.14159265f / 2.0f;
		FVector3f axis(0, 1, 0);
		M.SetRotation(axis, angle);

		FVector3f p(1, 0, 0);
		FVector3f result = M * p;

		assert(AlmostEqual(result.X, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 0.0f, 1e-4f));
		assert(AlmostEqual(result.Z, -1.0f, 1e-4f));
		std::cout << "  SetRotation: [PASSED]" << std::endl;
	}

	// SetTRS on existing matrix
	{
		FMatrix4x4f M;
		FVector3f translation(5, 10, 15);
		FVector3f rotationAxis(0, 0, 1);
		float rotationAngle = 0.0f;
		FVector3f scale(2, 3, 4);
		M.SetTRS(translation, rotationAxis, rotationAngle, scale);

		FVector3f p(1, 2, 3);
		FVector3f result = M * p;

		assert(AlmostEqual(result.X, 7.0f, 1e-4f));
		assert(AlmostEqual(result.Y, 16.0f, 1e-4f));
		assert(AlmostEqual(result.Z, 27.0f, 1e-4f));
		std::cout << "  SetTRS: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix4x4_Components()
{
	std::cout << "=== Testing TMatrix4x4 Component Extraction ===" << std::endl;

	// GetRotation3x3
	{
		float angle = 3.14159265f / 4.0f;
		FVector3f axis(1, 0, 0);
		FMatrix4x4f M = FMatrix4x4f::Rotation(axis, angle);

		FMatrix3x3f rot3x3 = M.GetRotation3x3();

		// Should match the rotational 3x3 matrix
		float c = std::cos(angle);
		float s = std::sin(angle);

		assert(AlmostEqual(rot3x3(1, 1), c, 1e-4f));
		assert(AlmostEqual(rot3x3(1, 2), -s, 1e-4f));
		assert(AlmostEqual(rot3x3(2, 1), s, 1e-4f));
		assert(AlmostEqual(rot3x3(2, 2), c, 1e-4f));
		std::cout << "  GetRotation3x3: [PASSED]" << std::endl;
	}

	// GetTranslation
	{
		FVector3f expectedTrans(7, 8, 9);
		FMatrix4x4f M = FMatrix4x4f::Translation(expectedTrans);

		FVector3f trans = M.GetTranslation();

		assert(AlmostEqual(trans.X, expectedTrans.X));
		assert(AlmostEqual(trans.Y, expectedTrans.Y));
		assert(AlmostEqual(trans.Z, expectedTrans.Z));
		std::cout << "  GetTranslation: [PASSED]" << std::endl;
	}

	// GetScale
	{
		FVector3f expectedScale(2.0f, 3.0f, 4.0f);
		FMatrix4x4f M = FMatrix4x4f::Scaling(expectedScale);

		FVector3f scale = M.GetScale();

		assert(AlmostEqual(scale.X, expectedScale.X));
		assert(AlmostEqual(scale.Y, expectedScale.Y));
		assert(AlmostEqual(scale.Z, expectedScale.Z));
		std::cout << "  GetScale: [PASSED]" << std::endl;
	}

	// Uniform scaling using scalar parameter
	{
		float uniformScale = 5.0f;
		FMatrix4x4f S = FMatrix4x4f::Scaling(uniformScale);
		FVector3f v(1, 1, 1);
		FVector3f result = S * v;

		assert(AlmostEqual(result.X, 5.0f));
		assert(AlmostEqual(result.Y, 5.0f));
		assert(AlmostEqual(result.Z, 5.0f));
		std::cout << "  Uniform scaling (scalar): [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

void TestMatrix4x4_Inverse_Advanced()
{
	std::cout << "=== Testing TMatrix4x4 Inverse (Advanced) ===" << std::endl;

	// Combined transformation inverse
	{
		FVector3f translation(5, 10, 15);
		FVector3f axis(0, 1, 0);
		float angle = 0.5f;
		FVector3f scale(2, 3, 4);

		FMatrix4x4f M = FMatrix4x4f::TRS(translation, axis, angle, scale);
		FMatrix4x4f MInv = M.Inverse();

		FMatrix4x4f Result = M * MInv;

		FMatrix4x4f I = FMatrix4x4f::Identity();
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				assert(AlmostEqual(Result(i, j), I(i, j), 1e-3f));
			}
		}
		std::cout << "  Combined transform inverse: [PASSED]" << std::endl;
	}

	// Singular matrix (non-invertible)
	{
		FMatrix3x3f singular(
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);

		// Build a 4x4 with singular linear part
		FMatrix4x4f M(
			FVector4f(singular.r0.X, singular.r0.Y, singular.r0.Z, 0),
			FVector4f(singular.r1.X, singular.r1.Y, singular.r1.Z, 0),
			FVector4f(singular.r2.X, singular.r2.Y, singular.r2.Z, 0),
			FVector4f(0, 0, 0, 1)
		);

		FMatrix4x4f MInv = M.Inverse();

		// Should return zero matrix for singular case
		assert(MInv == FMatrix4x4f::Zero());
		std::cout << "  Singular matrix handling: [PASSED]" << std::endl;
	}

	std::cout << std::endl;
}

int main(int argc, char** argv)
{
	std::cout << "========================================" << std::endl;
	std::cout << "=== UCommon Matrix Test Suite ===" << std::endl;
	std::cout << "========================================" << std::endl << std::endl;

	// TMatrix3x3 tests
	TestMatrix3x3_Constructors();
	TestMatrix3x3_VectorMultiplication();
	TestMatrix3x3_Rotation();
	TestMatrix3x3_Operations();
	TestMatrix3x3_UniformScaling();
	TestMatrix3x3_MatrixMultiplication();
	TestMatrix3x3_AdditionalOperations();

	// TMatrix4x4 tests
	TestMatrix4x4_Constructors();
	TestMatrix4x4_Transformations();
	TestMatrix4x4_Trs();
	TestMatrix4x4_Vector4();
	TestMatrix4x4_Inverse();
	TestMatrix4x4_Operations();
	TestMatrix4x4_SetMethods();
	TestMatrix4x4_Components();
	TestMatrix4x4_Inverse_Advanced();

	std::cout << "========================================" << std::endl;
	std::cout << "=== All Matrix tests passed! ===" << std::endl;
	std::cout << "========================================" << std::endl;

	return 0;
}
