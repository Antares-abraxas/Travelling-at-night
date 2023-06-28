#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <iostream>
#include <vector>

// ��������� ������������ ������ ��������������� �������
#include <fstream>
#include <json/json.h>
#include <cstdlib>
#include <ctime>

using namespace std;

// ...

// ������ ��������� ������ ��� ������� RandomEvent
TEST_CASE("RandomEvent test") {
    srand(0);
    std::vector<InventoryItem> testInventory;
    RandomEvent(testInventory);
    // �������� ��������� �����������
    CHECK_EQ(testInventory.size(), 1);
    CHECK_EQ(testInventory[0].name, "Sword");
    CHECK_EQ(testInventory[0].damageBonus, 10);
    CHECK_EQ(testInventory[0].armorBonus, 0);
    CHECK_EQ(testInventory[0].healthBonus, 0);
}

// ������ ��������� ������ ��� ������� Battle
TEST_CASE("Battle test") {
    srand(0);
    int testEnemyHealth = 50;
    Hero testHero;
    testHero.health = 100;
    std::vector<InventoryItem> testInventory;
    Battle(testEnemyHealth, testInventory, testHero);
    // �������� ��������� �����������
    CHECK_EQ(testHero.health, 80);
    CHECK_EQ(testEnemyHealth, 0);
}

// ������ ��������� ������ ��� ������� UseHealthPotion
TEST_CASE("UseHealthPotion test") {
    std::vector<InventoryItem> testInventory;
    Hero testHero;
    testHero.health = 50;
    UseHealthPotion(testInventory, testHero);
    // �������� ��������� �����������
    CHECK_EQ(testHero.health, 70);
    CHECK_EQ(testInventory[0].healthBonus, 0);
}

// ������ ��������� ������ ��� ������� DisplayInventory
TEST_CASE("DisplayInventory test") {
    std::vector<InventoryItem> testInventory;
    InventoryItem item1;
    item1.name = "Sword";
    item1.damageBonus = 10;
    item1.armorBonus = 0;
    item1.healthBonus = 0;
    testInventory.push_back(item1);

    InventoryItem item2;
    item2.name = "Shield";
    item2.damageBonus = 0;
    item2.armorBonus = 5;
    item2.healthBonus = 0;
    testInventory.push_back(item2);

    // �������� ��������� �����������
    std::ostringstream output;
    std::streambuf* coutBuffer = std::cout.rdbuf();
    std::cout.rdbuf(output.rdbuf());
    DisplayInventory(testInventory);
    std::cout.rdbuf(coutBuffer);
    std::string expectedOutput = "=== Inventory ===\n"
        "Item: Sword\n"
        "Damage Bonus: 10\n"
        "Armor Bonus: 0\n"
        "Health Bonus: 0\n"
        "\n"
        "Item: Shield\n"
        "Damage Bonus: 0\n"
        "Armor Bonus: 5\n"
        "Health Bonus: 0\n"
        "\n";
    CHECK_EQ(output.str(), expectedOutput);
}

// ... (������ �������� ������ ��� ������ �������)

int main(int argc, char** argv) {
    // ������ �������� ������� � �������������� doctest
    doctest::Context context(argc, argv);
    int result = context.run();

    return result;
}
