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

 /**
  * @struct StoryNode
  * @brief Structure for representing a story node.
  */
struct StoryNode {
    std::string hint;                ///< The hint or message associated with the story node.
    std::vector<StoryNode> nextNodes;///< The next story nodes to choose from.
    int enemyHealth;            ///< The health of the enemy in the story node.
};

/**
 * @struct InventoryItem
 * @brief Structure for representing an inventory item.
 */
struct InventoryItem {
    std::string name;        ///< The name of the inventory item.
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

StoryNode LoadStoryFromJSON(const std::string& filename) {
    std::ifstream file(filename);
    Json::Value root;
    file >> root;

    return BuildStoryNode(root);
}

/**
 * @brief Function for the random event.
 * @param inventory The player's inventory.
 */

void RandomEvent(std::vector<InventoryItem>& inventory) {
    srand(time(0));

    int event = rand() % 3;

    if (event == 0) {
        InventoryItem newItem;
        newItem.name = "Sword";
        newItem.damageBonus = 10;
        newItem.armorBonus = 0;
        newItem.healthBonus = 0;

        inventory.push_back(newItem);

        std::cout << "You found a sword and added it to your inventory!" << std::endl;
    }
    else if (event == 1) {
        if (!inventory.empty()) {
            int index = rand() % inventory.size();
            inventory.erase(inventory.begin() + index);

            std::cout << "You lost a random item from your inventory!" << std::endl;
        }
    }
    else {
        std::cout << "Nothing happens." << std::endl;
    }
}

/**
 * @brief Function for the battle.
 * @param enemyHealth The health of the enemy.
 * @param inventory The player's inventory.
 * @param hero The main hero.
 */
void Battle(int& enemyHealth, std::vector<InventoryItem>& inventory, Hero& hero) {
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

            std::cout << "You attacked the enemy and dealt " << totalPlayerDamage << " damage." << std::endl;
            std::cout << "The enemy attacked you and dealt " << enemyDamage << " damage." << std::endl;
            std::cout << "Your health: " << hero.health << std::endl;
        }
    }

    if (hero.health <= 0) {
        std::cout << "You lost the battle!" << std::endl;
        return;
    }

    std::cout << "Enemy defeated!" << std::endl;
}

/**
 * @brief Function for using health potions.
 * @param inventory The player's inventory.
 * @param hero The main hero.
 */

void UseHealthPotion(std::vector<InventoryItem>& inventory, Hero& hero) {
    for (auto& item : inventory) {
        if (item.name == "Health Potion" && item.healthBonus > 0) {
            item.healthBonus--;
            hero.health += 20;
            std::cout << "You used a health potion and restored some health!" << std::endl;
            return;
        }
    }

    std::cout << "You don't have any health potions!" << std::endl;
}

/**
 * @brief Function for displaying the inventory.
 * @param inventory The player's inventory.
 */
void DisplayInventory(const std::vector<InventoryItem>& inventory) {
    std::cout << "=== Inventory ===" << std::endl;
    for (const auto& item : inventory) {
        std::cout << "Item: " << item.name << std::endl;
        std::cout << "Damage Bonus: " << item.damageBonus << std::endl;
        std::cout << "Armor Bonus: " << item.armorBonus << std::endl;
        std::cout << "Health Bonus: " << item.healthBonus << std::endl;
        std::cout << std::endl;
    }
}

/**
 * @brief Function for playing the text-based adventure.
 * @param currentNode The current story node.
 * @param inventory The player's inventory.
 * @param hero The main hero.
 */
void PlayStory(StoryNode& currentNode, std::vector<InventoryItem>& inventory, Hero& hero) {
    std::cout << currentNode.hint << std::endl;

    if (currentNode.nextNodes.empty()) {
        std::cout << "=== End of the game ===" << std::endl;
        return;
    }

    char choice;
    std::cin >> choice;

    int choiceIndex = choice - 'A';

    if (choiceIndex >= 0 && choiceIndex < currentNode.nextNodes.size()) {
        StoryNode& nextNode = currentNode.nextNodes[choiceIndex];

        if (nextNode.enemyHealth > 0) {
            std::cout << "Engaging in battle with the enemy!" << std::endl;
            Battle(nextNode.enemyHealth, inventory, hero);
        }
        else {
            RandomEvent(inventory);
        }
        PlayStory(nextNode, inventory, hero);
    }
    else if (choice == 'H') {
        UseHealthPotion(inventory, hero);
        PlayStory(currentNode, inventory, hero);
    }
    else if (choice == 'I') {
        DisplayInventory(inventory);
        PlayStory(currentNode, inventory, hero);
    }
    else {
        std::cout << "Invalid choice. Please try again." << std::endl;
        PlayStory(currentNode, inventory, hero);
    }
}

/**
 * @brief Main function.
 * @return Exit code.
 */
int main() {
    std::string filename = "story.json";  
    StoryNode storyRoot = LoadStoryFromJSON(filename);

    std::vector<InventoryItem> inventory;  
    Hero hero;
    hero.health = 100;  
    PlayStory(storyRoot, inventory, hero);

    return 0;
}

