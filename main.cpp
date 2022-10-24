#include <iostream>
#include <string>
#include <fstream>
#include <vector>

const int MAP_SIZE = 20;
const int NUM_OF_PLAYERS = 6;
const int MAX_INT = 32767;

struct vec {
    int x = 0, y = 0;
};

enum control_key{//for enemy
    UP = 1,
    DOWN,
    LEFT,
    RIGHT
};

struct character {
    bool is_hero = true;
    std::string name;
    int health = 0;
    int armor = 0;
    int damage = 0;
    vec position;
};


//инициализация персонажей (ООП - конструктор)
character hero_init();
character enemy_init(int i);

//действия персонажей (ООП - методы)
void take_damage(character& person, int damage);
void move_hero(character& person, std::string direction);
void move_enemy(character& person);
void step_back(character& person, int prev_x, int prev_y);

//отрисовка карты
void map_painter(std::vector<character>& players);

//ввод - вывод info персонажей
void load_characters(std::vector<character>& characters);
void save_characters(std::vector<character>& characters);
void print_character(character &person);

//состояние игры
bool game_over(std::vector<character>& players);

//состояния персонажей
bool out_of_map(character& person);
bool is_alive(character& person);

//безопасность
std::string get_and_verify_command();
int get_verify_int();


int main() {
    std::cout << "21.4 Fighting game\n";

    std::vector<character> players;
    //инициализация игроков:
    for (int i = 0; i < NUM_OF_PLAYERS; ++i) {
        if(i == 0)
            players.push_back(hero_init());
        else
            players.push_back(enemy_init(i));
    };

    while(!game_over(players)){
        for(int i = 0; i < players.size(); i++){
            if(is_alive(players[i])){

                map_painter(players);

                std::cout << "Ход игрока: " << std::endl;
                print_character(players[i]);

                int pos_x = players[i].position.x;
                int pos_y = players[i].position.y;

                if(players[i].is_hero) {
                    std::string command;
                    std::cout << "Ваш ход 'w -s - a - d'/(load/save): ";
                    command = get_and_verify_command();

                    if (command == "load") {
                        load_characters(players);
                    } else if (command == "save") {
                        save_characters(players);
                    } else move_hero(players[i], command);
                } else{
                    move_enemy(players[i]);
                }

                if (out_of_map(players[i])){
                    std::cout << "Ход за пределы игрового поля недоступен." << std::endl;
                    step_back(players[i], pos_x, pos_y);
                    break;
                }
                for(int next = 0; next < players.size(); next++){ // проверяем ячейку на наличие живого противника
                    if((i != next) && is_alive(players[next])) {
                        //проверить не занята ли новая позиция другим игроком
                        if(players[i].position.x == players[next].position.x
                            && players[i].position.y == players[next].position.y){

                            if(players[i].is_hero == players[next].is_hero){
                                //если занята игроком того же флага, то вернуться
                                //на изначальную позицию и завершить ход
                                step_back(players[i], pos_x, pos_y);

                            } else { //если занята игроком с другим флагом - то сразиться
                                take_damage(players[next], players[i].damage);
                                if(is_alive(players[next])){
                                    step_back(players[i], pos_x, pos_y);
                                } else {
                                    std::cout << "Игрок " << players[next].name << " побежден!" << std::endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

character hero_init(){
    character person;
    std::cout << "Enter the name: ";
    std::cin >> person.name;
    std::cout << "Health: ";
    person.health = get_verify_int();
    std::cout << "Armor: ";
    person.armor = get_verify_int();
    std::cout << "Damage: ";
    person.damage  = get_verify_int();

    person.position.x = (std::rand() % 19);
    person.position.y = (std::rand() % 19);

    return person;
}

character enemy_init(int i){
    character person;
    person.is_hero = false;
    person.name = "Enemy " + std::to_string(i);
    person.health = (std::rand() % 100) + 50;
    person.armor = (std::rand() % 50);
    person.damage = (std::rand() % 15) + 15;
    person.position.x = (std::rand() % 19);
    person.position.y = (std::rand() % 19);

    return person;
}

void take_damage(character& person, int damage) {
    person.armor -= damage;
    if (person.armor < 0) {
        person.health += person.armor;
        person.armor = 0;
    }
}


void move_hero(character& person, std::string direction){
    if(direction == "w") 
      person.position.x--;
    else if(direction == "s")
      person.position.x++;
    else if(direction == "a") 
      person.position.y--;
    else if(direction == "d")
      person.position.y++;
}


void move_enemy(character& person){
    int direction = ((std::rand() % 4) + 1);
    switch(direction){
        case UP: person.position.x--; break;
        case DOWN: person.position.x++; break;
        case LEFT: person.position.y--; break;
        case RIGHT: person.position.y++; break;
    }
}

void step_back(character& person, int prev_x, int prev_y){
    person.position.x = prev_x;
    person.position.y = prev_y;
}


void print_character(character &person) {
    std::cout << "Name: " << person.name << std::endl;
    std::cout << "Health: " << person.health << std::endl;
    std::cout << "Armor: " << person.armor << std::endl;
    std::cout << "Damage: " << person.damage << std::endl;
    std::cout << "Position: " << person.position.x + 1;
    std::cout << ", " << person.position.y + 1 << std::endl;
}

void map_painter(std::vector <character>& players){
    int map[MAP_SIZE][MAP_SIZE] = {0}; //game field

    for(auto & player : players){
        if(is_alive(player)){
            if(player.is_hero){
                map[player.position.x][player.position.y] = 1;
            } else {
                map[player.position.x][player.position.y] = 2;
            }
        }
    }

    for(auto & i : map){
        for(int j : i){
            if(j == 0) std::cout << " . ";
            if(j == 1) std::cout << " P ";
            if(j == 2) std::cout << " E ";
        }
        std::cout << std::endl;
    }
}

void load_characters(std::vector <character>& characters){
    std::ifstream file("save.bin", std::ios::binary);
    if(file.is_open()){
        for (int i = 0; i < 6; ++i){
            file.read((char*)&characters[i].is_hero, sizeof(characters[i].is_hero));

            int name_len;
            file.read((char*)&name_len, sizeof(name_len));
            characters[i].name.resize(name_len);
            file.read((char*)characters[i].name.c_str(), name_len);

            file.read((char*)&characters[i].health, sizeof(characters[i].health));
            file.read((char*)&characters[i].armor, sizeof(characters[i].armor));
            file.read((char*)&characters[i].damage, sizeof(characters[i].damage));
            file.read((char*)&characters[i].position.x, sizeof(characters[i].position.x));
            file.read((char*)&characters[i].position.y, sizeof(characters[i].position.y));
        }
    } else {
        std::cout << "Error for opening file!" << std::endl;
    }
}

void save_characters(std::vector <character>& characters){
    std::ofstream file("save.bin", std::ios::binary);
    if(file.is_open()) {
        for (int i = 0; i < 6; ++i) {
            file.write((char *) &(characters[i].is_hero), sizeof(characters[i].is_hero));

            int name_len = characters[i].name.length();
            file.write((char *) &name_len, sizeof(name_len));
            file.write(characters[i].name.c_str(), name_len);

            file.write((char *) &(characters[i].health), sizeof(characters[i].health));
            file.write((char *) &(characters[i].armor), sizeof(characters[i].armor));
            file.write((char *) &(characters[i].damage), sizeof(characters[i].damage));
            file.write((char *) &(characters[i].position.x), sizeof(characters[i].position.x));
            file.write((char *) &(characters[i].position.y), sizeof(characters[i].position.y));
        }
    } else {
        std::cout << "Error for opening file!" << std::endl;
    }
}

bool game_over(std::vector <character>& players){
    int enemy_defeated_counter = 0;// возможно переменная должна быть глобальным счетчиком - инфо состояние игры.

    for(auto & player : players){
        if(!is_alive(player)){
            if(player.is_hero) {
                std::cout << "Game over. You lost((";
                return true;
            } else enemy_defeated_counter++;
        }
    }
    if (enemy_defeated_counter ==  NUM_OF_PLAYERS - 1) {
        std::cout << "Game over. You win!!))";
        return true;
    }
    return false;
}

bool out_of_map(character& person){
    if(person.position.x >= 20 || person.position.x < 0
       || person.position.y >= 20 || person.position.y < 0 ) return true;
    else return false;
}

bool is_alive(character& person){
    return person.health > 0;
}

std::string get_and_verify_command(){
    std::string command;
    std::cin >> command;
    while (command != "w" && command != "s" && command != "a" && command != "d"
           && command != "load" && command != "save") {
        std::cout << "Incorrect command. Try again: ";
        std::cin >> command;
    }
    return command;
}

int get_verify_int(){
    int n = 0;
    std::cin >> n;
    while(n < 0 || n > MAX_INT){
        std::cout << "Incorrect command. Try again: ";
        std::cin >> n;
    }
    return n;
}

