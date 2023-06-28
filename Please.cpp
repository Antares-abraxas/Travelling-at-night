/**
 * @file main.cpp
 * @brief Text-based adventure game.
 */

#include <iostream>
#include <fstream>
#include <json/json.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

 /**
  * @struct StoryNode
  * @brief Structure for representing a story node.
  */
struct StoryNode {
    string hint;                ///< The hint or message associated with the story node.
    vector<StoryNode> nextNodes;///< The next story nodes to choose from.
    int enemyHealth;            ///< The health of the enemy in the story node.
};

/**
 * @struct InventoryItem
 * @brief Structure for representing an inventory item.
 */
struct InventoryItem {
    string name;        ///< The name of the inventory item.
    int damageBonus;    ///< The damage bonus provided by the item.
    int armorBonus;     ///< The armor bonus provided by the item.
    int healthBonus;    ///< The health bonus provided by the item.
};

/**
 * @struct Hero
 * @brief Structure for representing the main hero.
 */
struct Hero {
    int health;         ///< The health of the hero.
};

/**
 * @brief Recursive function to build the story tree.
 * @param nodeData The JSON data for the current story node.
 * @return The constructed story node.
 */

StoryNode BuildStoryNode(const Json::Value& nodeData) {
    StoryNode node;
    node.hint = nodeData["hint"].asString();
    node.enemyHealth = nodeData["enemyHealth"].asInt();

    if (nodeData.isMember("choices")) {
        const Json::Value& choicesData = nodeData["choices"];
        for (const auto& choiceData : choicesData) {
            StoryNode nextNode = BuildStoryNode(choiceData);
            node.nextNodes.push_back(nextNode);
        }
    }

    return node;
}

/**
 * @brief Function to load the story from a JSON file.
 * @param filename The path to the JSON file.
 * @return The root story node.
 */

StoryNode LoadStoryFromJSON(const string& filename) {
    ifstream file(filename);
    Json::Value root;
    file >> root;

    return BuildStoryNode(root);
}

/**
 * @brief Function for the random event.
 * @param inventory The player's inventory.
 */

void RandomEvent(vector<InventoryItem>& inventory) {
    srand(time(0));

    int event = rand() % 3;

    if (event == 0) {
        // Gain an item
        InventoryItem newItem;
        newItem.name = "Sword";
        newItem.damageBonus = 10;
        newItem.armorBonus = 0;
        newItem.healthBonus = 0;

        inventory.push_back(newItem);

        cout << "You found a sword and added it to your inventory!" << endl;
    }
    else if (event == 1) {
        if (!inventory.empty()) {
            int index = rand() % inventory.size();
            inventory.erase(inventory.begin() + index);

            cout << "You lost a random item from your inventory!" << endl;
        }
    }
    else {
        cout << "Nothing happens." << endl;
    }
}

/**
 * @brief Function for the battle.
 * @param enemyHealth The health of the enemy.
 * @param inventory The player's inventory.
 * @param hero The main hero.
 */
void Battle(int& enemyHealth, vector<InventoryItem>& inventory, Hero& hero) {
    srand(time(0));

    int totalPlayerDamage = 2;
    int totalPlayerArmor = 0;

    for (const auto& item : inventory) {
        totalPlayerDamage += item.damageBonus;
        totalPlayerArmor += item.armorBonus;
    }

    int enemyDamage = rand() % 10 + 1;

    while (enemyHealth > 0 && hero.health > 0) {
        enemyHealth -= totalPlayerDamage;
        if (enemyHealth > 0) {
            hero.health -= enemyDamage;

            cout << "You attacked the enemy and dealt " << totalPlayerDamage << " damage." << endl;
            cout << "The enemy attacked you and dealt " << enemyDamage << " damage." << endl;
            cout << "Your health: " << hero.health << endl;
        }
    }

    if (hero.health <= 0) {
        cout << "You lost the battle!" << endl;
        return;
    }

    cout << "Enemy defeated!" << endl;
}

/**
 * @brief Function for using health potions.
 * @param inventory The player's inventory.
 * @param hero The main hero.
 */

void UseHealthPotion(vector<InventoryItem>& inventory, Hero& hero) {
    for (auto& item : inventory) {
        if (item.name == "Health Potion" && item.healthBonus > 0) {
            item.healthBonus--;
            hero.health += 20;
            cout << "You used a health potion and restored some health!" << endl;
            return;
        }
    }

    cout << "You don't have any health potions!" << endl;
}

/**
 * @brief Function for displaying the inventory.
 * @param inventory The player's inventory.
 */
void DisplayInventory(const vector<InventoryItem>& inventory) {
    cout << "=== Inventory ===" << endl;
    for (const auto& item : inventory) {
        cout << "Item: " << item.name << endl;
        cout << "Damage Bonus: " << item.damageBonus << endl;
        cout << "Armor Bonus: " << item.armorBonus << endl;
        cout << "Health Bonus: " << item.healthBonus << endl;
        cout << endl;
    }
}

/**
 * @brief Function for playing the text-based adventure.
 * @param currentNode The current story node.
 * @param inventory The player's inventory.
 * @param hero The main hero.
 */
void PlayStory(StoryNode& currentNode, vector<InventoryItem>& inventory, Hero& hero) {
    cout << currentNode.hint << endl;

    if (currentNode.nextNodes.empty()) {
        cout << "=== End of the game ===" << endl;
        return;
    }

    char choice;
    cin >> choice;

    int choiceIndex = choice - 'A';

    if (choiceIndex >= 0 && choiceIndex < currentNode.nextNodes.size()) {
        StoryNode& nextNode = currentNode.nextNodes[choiceIndex];

        if (nextNode.enemyHealth > 0) {
            // Battle with the enemy
            cout << "Engaging in battle with the enemy!" << endl;
            Battle(nextNode.enemyHealth, inventory, hero);
        }
        else {
            // Random event
            RandomEvent(inventory);
        }
        PlayStory(nextNode, inventory, hero);
    }
    else if (choice == 'H') {
        // Use health potion
        UseHealthPotion(inventory, hero);
        PlayStory(currentNode, inventory, hero);
    }
    else if (choice == 'I') {
        // Display inventory
        DisplayInventory(inventory);
        PlayStory(currentNode, inventory, hero);
    }
    else {
        cout << "Invalid choice. Please try again." << endl;
        PlayStory(currentNode, inventory, hero);
    }
}

/**
 * @brief Main function.
 * @return Exit code.
 */
int main() {
    string filename = "story.json";  
    StoryNode storyRoot = LoadStoryFromJSON(filename);

    vector<InventoryItem> inventory;  
    Hero hero;
    hero.health = 100;  
    PlayStory(storyRoot, inventory, hero);

    return 0;
}

