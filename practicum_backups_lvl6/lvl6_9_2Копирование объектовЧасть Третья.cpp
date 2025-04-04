#include "octopus.h"

#include <cassert>
#include <iostream>

using namespace std;

int main() {
    // �������� ��������������� ����������
    {
        // �� ��������� �������� ����� 8 �������
        Octopus default_octopus;
        assert(default_octopus.GetTentacleCount() == 8);

        // �������� ����� ����� �������� �� 8 ���������� �������
        Octopus quadropus(4);
        assert(quadropus.GetTentacleCount() == 4);

        // � ���� ������ �� ����� �������
        Octopus coloboque(0);
        assert(coloboque.GetTentacleCount() == 0);
    }

    // ��������� ����� ��������� ��������
    {
        Octopus octopus(1);
        Tentacle* t0 = &octopus.GetTentacle(0);
        Tentacle* t1 = &octopus.AddTentacle();
        assert(octopus.GetTentacleCount() == 2);
        Tentacle* t2 = &octopus.AddTentacle();
        assert(octopus.GetTentacleCount() == 3);

        // ����� ���������� ������� ����� ��������� �������� �� ������ ����� �������
        assert(&octopus.GetTentacle(0) == t0);
        assert(&octopus.GetTentacle(1) == t1);
        assert(&octopus.GetTentacle(2) == t2);

        for (int i = 0; i < octopus.GetTentacleCount(); ++i) {
            assert(octopus.GetTentacle(i).GetId() == i + 1);
        }
    }

    // ��������� ����� ������������ � ��������� ���� �����
    {
        Octopus male(2);
        Octopus female(2);

        assert(male.GetTentacle(0).GetLinkedTentacle() == nullptr);

        male.GetTentacle(0).LinkTo(female.GetTentacle(1));
        assert(male.GetTentacle(0).GetLinkedTentacle() == &female.GetTentacle(1));

        male.GetTentacle(0).Unlink();
        assert(male.GetTentacle(0).GetLinkedTentacle() == nullptr);
    }

    // ����� ��������� ����� ���� ����������� ����� �������, �������
    // �������� ��������� ������� ������������� ���������
    {
        // ���������� ���������� � ������ ����������� �������
        for (int num_tentacles = 0; num_tentacles < 10; ++num_tentacles) {
            Octopus male(num_tentacles);
            Octopus female(num_tentacles);
            // ����� ��� ������� ���� ����� �� ��������
            for (int i = 0; i < num_tentacles; ++i) {
                male.GetTentacle(i).LinkTo(female.GetTentacle(num_tentacles - 1 - i));
            }

            Octopus male_copy(male);
            // ��������� ��������� ������� �����
            assert(male_copy.GetTentacleCount() == male.GetTentacleCount());
            for (int i = 0; i < male_copy.GetTentacleCount(); ++i) {
                // ������ �������� ����� ����������� �� ������, ��������� �� ������ ������������� ��������
                assert(&male_copy.GetTentacle(i) != &male.GetTentacle(i));
                // ������ �������� ����� ������������ � ���� �� ��������, ��� � ������������
                assert(male_copy.GetTentacle(i).GetLinkedTentacle() == male.GetTentacle(i).GetLinkedTentacle());
            }
        }
        // ���� �� ������ ��� �������, �� ���������� ����������, ������ �����,
        // ������ ��� ��������������� ���������
        cout << "Everything is OK" << endl;
    }

    return 0;
}