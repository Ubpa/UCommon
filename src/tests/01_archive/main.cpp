#include <UCommon/Archive.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <UCommon_ext/doctest/doctest.h>

using namespace UCommon;

struct A0
{
    float data;
    void Serialize(IArchive& Archive)
    {
        Archive.ByteSerialize(data);
    }
};

struct A1
{
    double data;
    void Serialize(IArchive& Archive)
    {
        Archive.UseVersion(0xFFFFAAAAFFFFAAAA, 0);
        int64_t ver = Archive.GetVersion(0xFFFFAAAAFFFFAAAA);
        if (ver < 0)
        {
            A0 a0;
            a0.Serialize(Archive);
            data = a0.data;
        }
        else
        {
            Archive.ByteSerialize(data);
        }
    }
};

struct A2
{
    int data;
    void Serialize(IArchive& Archive)
    {
        Archive.UseVersion(0xFFFFAAAAFFFFAAAA, 1);
        int64_t ver = Archive.GetVersion(0xFFFFAAAAFFFFAAAA);
        if (ver < 1)
        {
            A1 a1;
            a1.Serialize(Archive);
            data = (int)a1.data;
        }
        else
        {
            Archive.ByteSerialize(data);
        }
    }
};

TEST_CASE("Archive - A0 save and A1 load (version upgrade)")
{
    constexpr float Pi = 3.14159265358979323846f;

    // Save A0
    {
        A0 a0;
        a0.data = Pi;
        FFileArchive ar(IArchive::EState::Saving, "test_01_archive_a0.uasset");
        a0.Serialize(ar);
    }

    // Load as A1 (version upgrade: float -> double)
    A1 a1;
    {
        FFileArchive ar(IArchive::EState::Loading, "test_01_archive_a0.uasset");
        a1.Serialize(ar);
    }
    CHECK(a1.data == doctest::Approx((double)Pi).epsilon(1e-6));

    // Modify and save A1
    a1.data *= 9.8;
    {
        FFileArchive ar(IArchive::EState::Saving, "test_01_archive_a1.uasset");
        a1.Serialize(ar);
    }
}

TEST_CASE("Archive - A2 load from A1 (version upgrade)")
{
    constexpr float Pi = 3.14159265358979323846f;
    double expectedA1 = (double)Pi * 9.8;

    A2 a2;
    {
        FFileArchive ar(IArchive::EState::Loading, "test_01_archive_a1.uasset");
        a2.Serialize(ar);
    }
    CHECK(a2.data == (int)expectedA1);
}
