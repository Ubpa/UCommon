#include <UCommon/Archive.h>
#include <iostream>

using namespace UCommon;
using namespace std;

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

int main(int argc, char** argv)
{
    {
        A0 a0;
        a0.data = 3.14159265358979323846f;
        FFileArchive ar(IArchive::EState::Saving, "test_01_archive_a0.uasset");
        a0.Serialize(ar);
    }
    {
        A1 a1;
        FFileArchive ar_r(IArchive::EState::Loading, "test_01_archive_a0.uasset");
        a1.Serialize(ar_r);
        cout << a1.data << endl;
        a1.data *= 9.8;
        FFileArchive ar_w(IArchive::EState::Saving, "test_01_archive_a1.uasset");
        a1.Serialize(ar_w);
    }
    {
        A2 a2;
        FFileArchive ar(IArchive::EState::Loading, "test_01_archive_a1.uasset");
        a2.Serialize(ar);
        cout << a2.data << endl;
    }
    return 0;
}
