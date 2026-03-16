#include <UCommon/Matrix.h>
#include <UCommon/Vector.h>
#include <cmath>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <UCommon_ext/doctest/doctest.h>

using namespace UCommon;

TEST_CASE("Matrix3x3 - Constructors")
{
	// Default constructor (uninitialized)
	{
		FMatrix3x3f M;
		// Just check it compiles
		REQUIRE(true);
	}

	// Value constructor
	{
		FMatrix3x3f M(2.0f);
		CHECK(M(0, 0) == 2.0f);
		CHECK(M(1, 1) == 2.0f);
		CHECK(M(2, 2) == 2.0f);
	}

	// Element-wise constructor
	{
		FMatrix3x3f M(
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);
		CHECK(M(0, 0) == 1);
		CHECK(M(0, 1) == 2);
		CHECK(M(0, 2) == 3);
		CHECK(M(1, 0) == 4);
		CHECK(M(1, 1) == 5);
		CHECK(M(1, 2) == 6);
		CHECK(M(2, 0) == 7);
		CHECK(M(2, 1) == 8);
		CHECK(M(2, 2) == 9);
	}

	// Row vector constructor
	{
		FMatrix3x3f M(
			FVector3f(1, 2, 3),
			FVector3f(4, 5, 6),
			FVector3f(7, 8, 9)
		);
		CHECK(M[0].X == 1);
		CHECK(M[0].Y == 2);
		CHECK(M[0].Z == 3);
		CHECK(M[1].X == 4);
		CHECK(M[1].Y == 5);
		CHECK(M[1].Z == 6);
		CHECK(M[2].X == 7);
		CHECK(M[2].Y == 8);
		CHECK(M[2].Z == 9);
	}

	// Identity matrix
	{
		FMatrix3x3f M = FMatrix3x3f::Identity();
		CHECK(M(0, 0) == 1);
		CHECK(M(1, 1) == 1);
		CHECK(M(2, 2) == 1);
		CHECK(M(0, 1) == 0);
		CHECK(M(0, 2) == 0);
		CHECK(M(1, 0) == 0);
		CHECK(M(1, 2) == 0);
		CHECK(M(2, 0) == 0);
		CHECK(M(2, 1) == 0);
	}

	// Zero matrix
	{
		FMatrix3x3f M = FMatrix3x3f::Zero();
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				CHECK(M(i, j) == 0.0f);
			}
		}
	}
}

TEST_CASE("Matrix3x3 - Vector Multiplication")
{
	// Identity * vector = vector
	{
		FMatrix3x3f I = FMatrix3x3f::Identity();
		FVector3f v(1, 2, 3);
		FVector3f result = I * v;

		CHECK(result.X == doctest::Approx(v.X));
		CHECK(result.Y == doctest::Approx(v.Y));
		CHECK(result.Z == doctest::Approx(v.Z));
	}

	// Scaling matrix
	{
		FMatrix3x3f S = FMatrix3x3f::Scaling(FVector3f(2.0f, 3.0f, 4.0f));
		FVector3f v(1, 1, 1);
		FVector3f result = S * v;

		CHECK(result.X == doctest::Approx(2));
		CHECK(result.Y == doctest::Approx(3));
		CHECK(result.Z == doctest::Approx(4));
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
		CHECK(result.X == doctest::Approx(7.0f));
		CHECK(result.Y == doctest::Approx(13.0f));
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
		CHECK(result.X == doctest::Approx(2.0f));
		CHECK(result.Y == doctest::Approx(6.0f));
	}
}

TEST_CASE("Matrix3x3 - Rotation")
{
	// Rotation around X axis (90 degrees = pi/2)
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix3x3f M = FMatrix3x3f::RotationX(angle);
		FVector3f v(0, 1, 0);
		FVector3f result = M * v;

		// Point on Y axis should rotate to Z axis
		CHECK(result.X == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(1.0f).epsilon(1e-4f));
	}

	// Rotation around Y axis (90 degrees)
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix3x3f M = FMatrix3x3f::RotationY(angle);
		FVector3f v(1, 0, 0);
		FVector3f result = M * v;

		// Point on X axis should rotate to -Z axis
		CHECK(result.X == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(-1.0f).epsilon(1e-4f));
	}

	// Rotation around Z axis (90 degrees)
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix3x3f M = FMatrix3x3f::RotationZ(angle);
		FVector3f v(1, 0, 0);
		FVector3f result = M * v;

		// Point on X axis should rotate to Y axis
		CHECK(result.X == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(1.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(0.0f).epsilon(1e-4f));
	}

	// Axis-angle rotation (rotate (1,0,0) around (0,1,0) by 90°)
	{
		float angle = 3.14159265f / 2.0f;
		FVector3f axis(0, 1, 0);
		FMatrix3x3f M = FMatrix3x3f::Rotation(axis, angle);
		FVector3f v(1, 0, 0);
		FVector3f result = M * v;

		// Should be (0, 0, -1)
		CHECK(result.X == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(-1.0f).epsilon(1e-4f));
	}

	// 360 degree rotation should return identity
	{
		float angle = 2.0f * 3.14159265f;
		FVector3f axis(1, 1, 1);
		FMatrix3x3f M = FMatrix3x3f::Rotation(axis.SafeNormalize(), angle);
		FVector3f v(1, 2, 3);
		FVector3f result = M * v;

		CHECK(result.X == doctest::Approx(v.X).epsilon(1e-3f));
		CHECK(result.Y == doctest::Approx(v.Y).epsilon(1e-3f));
		CHECK(result.Z == doctest::Approx(v.Z).epsilon(1e-3f));
	}
}

TEST_CASE("Matrix3x3 - Operations")
{
	// Transpose
	{
		FMatrix3x3f M(
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);
		FMatrix3x3f MT = M.Transpose();

		CHECK(MT(0, 0) == M(0, 0));
		CHECK(MT(1, 1) == M(1, 1));
		CHECK(MT(2, 2) == M(2, 2));
		CHECK(MT(0, 1) == M(1, 0));
		CHECK(MT(0, 2) == M(2, 0));
		CHECK(MT(1, 0) == M(0, 1));
		CHECK(MT(1, 2) == M(2, 1));
		CHECK(MT(2, 0) == M(0, 2));
		CHECK(MT(2, 1) == M(1, 2));
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

		CHECK(C(0, 0) == doctest::Approx(A(0, 0)));
		CHECK(C(1, 1) == doctest::Approx(A(1, 1)));
		CHECK(C(2, 2) == doctest::Approx(A(2, 2)));
	}

	// Determinant of identity
	{
		FMatrix3x3f I = FMatrix3x3f::Identity();
		float det = I.Determinant();

		CHECK(det == doctest::Approx(1.0f));
	}

	// Trace
	{
		FMatrix3x3f M(
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);
		float trace = M.Trace();

		CHECK(trace == doctest::Approx(15.0f)); // 1 + 5 + 9
	}

	// Inverse of identity is identity
	{
		FMatrix3x3f I = FMatrix3x3f::Identity();
		FMatrix3x3f IInv = I.Inverse();

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				CHECK(I(i, j) == doctest::Approx(IInv(i, j)).epsilon(1e-5f));
			}
		}
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
				CHECK(Result(i, j) == doctest::Approx(expected).epsilon(1e-4f));
			}
		}
	}

	// Matrix comparison
	{
		FMatrix3x3f A = FMatrix3x3f::Identity();
		FMatrix3x3f B = FMatrix3x3f::Identity();
		FMatrix3x3f C = FMatrix3x3f::Zero();

		CHECK(A == B);
		CHECK(A != C);
	}
}

TEST_CASE("Matrix4x4 - Constructors")
{
	// Default constructor
	{
		FMatrix4x4f M;
		CHECK(true);
	}

	// Value constructor
	{
		FMatrix4x4f M(3.0f);
		CHECK(M(0, 0) == 3.0f);
		CHECK(M(1, 1) == 3.0f);
		CHECK(M(2, 2) == 3.0f);
		CHECK(M(3, 3) == 3.0f);
	}

	// Element-wise constructor
	{
		FMatrix4x4f M(
			1, 2, 3, 4,
			5, 6, 7, 8,
			9, 10, 11, 12,
			13, 14, 15, 16
		);
		CHECK(M(0, 0) == 1);
		CHECK(M(0, 3) == 4);
		CHECK(M(1, 0) == 5);
		CHECK(M(1, 3) == 8);
		CHECK(M(2, 0) == 9);
		CHECK(M(2, 3) == 12);
		CHECK(M(3, 0) == 13);
		CHECK(M(3, 3) == 16);
	}

	// Row vector constructor
	{
		FMatrix4x4f M(
			FVector4f(1, 2, 3, 4),
			FVector4f(5, 6, 7, 8),
			FVector4f(9, 10, 11, 12),
			FVector4f(13, 14, 15, 16)
		);
		CHECK(M[0].X == 1);
		CHECK(M[0].W == 4);
		CHECK(M[1].X == 5);
		CHECK(M[1].W == 8);
		CHECK(M[2].X == 9);
		CHECK(M[2].W == 12);
		CHECK(M[3].X == 13);
		CHECK(M[3].W == 16);
	}

	// Identity and Zero
	{
		FMatrix4x4f I = FMatrix4x4f::Identity();
		FMatrix4x4f Z = FMatrix4x4f::Zero();
		CHECK(I(0, 0) == 1);
		CHECK(I(1, 1) == 1);
		CHECK(I(2, 2) == 1);
		CHECK(I(3, 3) == 1);
		CHECK(Z(0, 0) == 0);
		CHECK(Z(3, 3) == 0);
	}
}

TEST_CASE("Matrix4x4 - Transformations")
{
	// Translation
	{
		FVector3f t(5, 10, 15);
		FMatrix4x4f T = FMatrix4x4f::Translation(t);
		FVector3f p(1, 2, 3);
		FVector3f result = T * p;

		CHECK(result.X == doctest::Approx(6.0f));
		CHECK(result.Y == doctest::Approx(12.0f));
		CHECK(result.Z == doctest::Approx(18.0f));
	}

	// Scaling
	{
		FMatrix4x4f S = FMatrix4x4f::Scaling(FVector3f(2, 3, 4));
		FVector3f p(1, 1, 1);
		FVector3f result = S * p;

		CHECK(result.X == doctest::Approx(2));
		CHECK(result.Y == doctest::Approx(3));
		CHECK(result.Z == doctest::Approx(4));
	}

	// Rotation around X axis
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix4x4f R = FMatrix4x4f::RotationX(angle);
		FVector3f p(0, 1, 0);
		FVector3f result = R * p;

		CHECK(result.X == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(1.0f).epsilon(1e-4f));
	}

	// Rotation around Y axis
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix4x4f R = FMatrix4x4f::RotationY(angle);
		FVector3f p(1, 0, 0);
		FVector3f result = R * p;

		CHECK(result.X == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(-1.0f).epsilon(1e-4f));
	}

	// Rotation around Z axis
	{
		float angle = 3.14159265f / 2.0f;
		FMatrix4x4f R = FMatrix4x4f::RotationZ(angle);
		FVector3f p(1, 0, 0);
		FVector3f result = R * p;

		CHECK(result.X == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(1.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(0.0f).epsilon(1e-4f));
	}

	// Axis-angle rotation (4x4)
	{
		float angle = 3.14159265f / 2.0f;
		FVector3f axis(0, 1, 0);
		FMatrix4x4f R = FMatrix4x4f::Rotation(axis, angle);
		FVector3f p(1, 0, 0);
		FVector3f result = R * p;

		CHECK(result.X == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(-1.0f).epsilon(1e-4f));
	}

	// Vector transformation (w=0, no translation)
	{
		FMatrix4x4f T = FMatrix4x4f::Translation(FVector3f(10, 20, 30));
		FVector3f v(1, 0, 0);
		FVector3f result = T.TransformVector(v);

		CHECK(result.X == doctest::Approx(1.0f));
		CHECK(result.Y == doctest::Approx(0.0f));
		CHECK(result.Z == doctest::Approx(0.0f));
	}
}

TEST_CASE("Matrix4x4 - TRS")
{
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
		CHECK(result.X == doctest::Approx(7.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(16.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(27.0f).epsilon(1e-4f));
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

		CHECK(result.X == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(-1.0f).epsilon(1e-4f));
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

		CHECK(extractedTrans.X == doctest::Approx(1.0f).epsilon(1e-4f));
		CHECK(extractedTrans.Y == doctest::Approx(2.0f).epsilon(1e-4f));
		CHECK(extractedTrans.Z == doctest::Approx(3.0f).epsilon(1e-4f));

		CHECK(extractedScale.X == doctest::Approx(2.0f).epsilon(1e-4f));
		CHECK(extractedScale.Y == doctest::Approx(3.0f).epsilon(1e-4f));
		CHECK(extractedScale.Z == doctest::Approx(4.0f).epsilon(1e-4f));
	}
}

TEST_CASE("Matrix4x4 - Vector4")
{
	// Transform TVector4
	{
		FMatrix4x4f T = FMatrix4x4f::Translation(FVector3f(5, 10, 15));
		FVector4f v(1, 2, 3, 1);  // w=1 means point
		FVector4f result = T * v;

		CHECK(result.X == doctest::Approx(6.0f));
		CHECK(result.Y == doctest::Approx(12.0f));
		CHECK(result.Z == doctest::Approx(18.0f));
		CHECK(result.W == doctest::Approx(1.0f));
	}

	// Transform TVector4 with w=0 (vector)
	{
		FMatrix4x4f T = FMatrix4x4f::Translation(FVector3f(5, 10, 15));
		FVector4f v(1, 2, 3, 0);  // w=0 means vector
		FVector4f result = T * v;

		// Translation should not affect
		CHECK(result.X == doctest::Approx(1.0f));
		CHECK(result.Y == doctest::Approx(2.0f));
		CHECK(result.Z == doctest::Approx(3.0f));
		CHECK(result.W == doctest::Approx(0.0f));
	}
}

TEST_CASE("Matrix4x4 - Inverse")
{
	// Inverse of identity is identity
	{
		FMatrix4x4f I = FMatrix4x4f::Identity();
		FMatrix4x4f IInv = I.Inverse();

		CHECK(I(0, 0) == IInv(0, 0));
		CHECK(I(3, 3) == IInv(3, 3));
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
				CHECK(Result(i, j) == doctest::Approx(I(i, j)).epsilon(1e-4f));
			}
		}
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
				CHECK(Result(i, j) == doctest::Approx(I(i, j)).epsilon(1e-4f));
			}
		}
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
				CHECK(Result(i, j) == doctest::Approx(expected).epsilon(1e-3f));
			}
		}
	}
}

TEST_CASE("Matrix3x3 - Uniform Scaling")
{
	// Uniform scaling using scalar parameter
	{
		float uniformScale = 3.0f;
		FMatrix3x3f S = FMatrix3x3f::Scaling(uniformScale);
		FVector3f v(1, 2, 3);
		FVector3f result = S * v;

		CHECK(result.X == doctest::Approx(3.0f));
		CHECK(result.Y == doctest::Approx(6.0f));
		CHECK(result.Z == doctest::Approx(9.0f));
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
				CHECK(S1(i, j) == doctest::Approx(S2(i, j)));
			}
		}
	}
}

TEST_CASE("Matrix3x3 - Matrix Multiplication")
{
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

		// C(0,0) = 1*9 + 2*6 + 3*3 = 9 + 12 + 9 = 30
		// C(0,1) = 1*8 + 2*5 + 3*2 = 8 + 10 + 6 = 24
		// C(0,2) = 1*7 + 2*4 + 3*1 = 7 + 8 + 3 = 18
		CHECK(C(0, 0) == doctest::Approx(30.0f));
		CHECK(C(0, 1) == doctest::Approx(24.0f));
		CHECK(C(0, 2) == doctest::Approx(18.0f));

		// C(1,0) = 4*9 + 5*6 + 6*3 = 36 + 30 + 18 = 84
		CHECK(C(1, 0) == doctest::Approx(84.0f));
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
				if (std::abs(AB(i, j) - BA(i, j)) > 1e-5f)
					isEqual = false;
			}
		}
		CHECK_FALSE(isEqual);
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
				CHECK(Result(i, j) == doctest::Approx(expected).epsilon(1e-4f));
			}
		}
	}
}

TEST_CASE("Matrix3x3 - Additional Operations")
{
	// Determinant of diagonal matrix
	{
		FMatrix3x3f M(
			2, 0, 0,
			0, 3, 0,
			0, 0, 4
		);
		float det = M.Determinant();
		CHECK(det == doctest::Approx(24.0f)); // 2 * 3 * 4
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
		CHECK(det == doctest::Approx(1.0f));
	}

	// Inverse of scaling matrix
	{
		FMatrix3x3f S = FMatrix3x3f::Scaling(FVector3f(2, 3, 4));
		FMatrix3x3f SInv = S.Inverse();

		// Check diagonal values
		CHECK(SInv(0, 0) == doctest::Approx(0.5f));
		CHECK(SInv(1, 1) == doctest::Approx(1.0f / 3.0f));
		CHECK(SInv(2, 2) == doctest::Approx(0.25f));
	}
}

TEST_CASE("Matrix4x4 - Operations")
{
	// Transpose
	{
		FMatrix4x4f M(
			1, 2, 3, 4,
			5, 6, 7, 8,
			9, 10, 11, 12,
			13, 14, 15, 16
		);
		FMatrix4x4f MT = M.Transpose();

		CHECK(MT(0, 0) == 1);
		CHECK(MT(3, 3) == 16);
		CHECK(MT(0, 3) == 13);
		CHECK(MT(3, 0) == 4);
		CHECK(MT(1, 0) == 2);
		CHECK(MT(0, 1) == 5);
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
		CHECK(det == doctest::Approx(24.0f)); // 2 * 3 * 4 * 1
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
		CHECK(trace == doctest::Approx(34.0f));
	}

	// Matrix comparison
	{
		FMatrix4x4f A = FMatrix4x4f::Identity();
		FMatrix4x4f B = FMatrix4x4f::Identity();
		FMatrix4x4f C = FMatrix4x4f::Zero();

		CHECK(A == B);
		CHECK(A != C);
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
		CHECK(result.X == doctest::Approx(4.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(6.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(12.0f).epsilon(1e-4f));
	}
}

TEST_CASE("Matrix4x4 - Set Methods")
{
	// SetRotation on existing matrix
	{
		FMatrix4x4f M = FMatrix4x4f::Zero();
		float angle = 3.14159265f / 2.0f;
		FVector3f axis(0, 1, 0);
		M.SetRotation(axis, angle);

		FVector3f p(1, 0, 0);
		FVector3f result = M * p;

		CHECK(result.X == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(0.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(-1.0f).epsilon(1e-4f));
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

		CHECK(result.X == doctest::Approx(7.0f).epsilon(1e-4f));
		CHECK(result.Y == doctest::Approx(16.0f).epsilon(1e-4f));
		CHECK(result.Z == doctest::Approx(27.0f).epsilon(1e-4f));
	}
}

TEST_CASE("Matrix4x4 - Component Extraction")
{
	// GetRotation3x3
	{
		float angle = 3.14159265f / 4.0f;
		FVector3f axis(1, 0, 0);
		FMatrix4x4f M = FMatrix4x4f::Rotation(axis, angle);

		FMatrix3x3f rot3x3 = M.GetRotation3x3();

		// Should match the rotational 3x3 matrix
		float c = std::cos(angle);
		float s = std::sin(angle);

		CHECK(rot3x3(1, 1) == doctest::Approx(c).epsilon(1e-4f));
		CHECK(rot3x3(1, 2) == doctest::Approx(-s).epsilon(1e-4f));
		CHECK(rot3x3(2, 1) == doctest::Approx(s).epsilon(1e-4f));
		CHECK(rot3x3(2, 2) == doctest::Approx(c).epsilon(1e-4f));
	}

	// GetTranslation
	{
		FVector3f expectedTrans(7, 8, 9);
		FMatrix4x4f M = FMatrix4x4f::Translation(expectedTrans);

		FVector3f trans = M.GetTranslation();

		CHECK(trans.X == doctest::Approx(expectedTrans.X));
		CHECK(trans.Y == doctest::Approx(expectedTrans.Y));
		CHECK(trans.Z == doctest::Approx(expectedTrans.Z));
	}

	// GetScale
	{
		FVector3f expectedScale(2.0f, 3.0f, 4.0f);
		FMatrix4x4f M = FMatrix4x4f::Scaling(expectedScale);

		FVector3f scale = M.GetScale();

		CHECK(scale.X == doctest::Approx(expectedScale.X));
		CHECK(scale.Y == doctest::Approx(expectedScale.Y));
		CHECK(scale.Z == doctest::Approx(expectedScale.Z));
	}

	// Uniform scaling using scalar parameter
	{
		float uniformScale = 5.0f;
		FMatrix4x4f S = FMatrix4x4f::Scaling(uniformScale);
		FVector3f v(1, 1, 1);
		FVector3f result = S * v;

		CHECK(result.X == doctest::Approx(5.0f));
		CHECK(result.Y == doctest::Approx(5.0f));
		CHECK(result.Z == doctest::Approx(5.0f));
	}
}

TEST_CASE("Matrix4x4 - Inverse Advanced")
{
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
				CHECK(Result(i, j) == doctest::Approx(I(i, j)).epsilon(1e-3f));
			}
		}
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
		CHECK(MInv == FMatrix4x4f::Zero());
	}
}
