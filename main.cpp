#include <iostream>
#include <string>
#include <fstream>
#include <vector>

const int MAP_SIZE = 20;
const int NUM_OF_PLAYERS = 6;

struct vec {
    int x = 0, y = 0;
};

enum control_key{//for enemy
    UP = 1,
    DOWN,
    LEFT,
    RIGHT
};

struct key{//for hero пока не используется
    char up = 'w';
    char down = 's';
    char left = 'a';
    char right = 'd';
};

struct character {
    bool flag = true;
   // bool playing = true;
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
void move_enemy(character& person);
void move_hero(character& person);

//отрисовка карты
void map_painter(std::vector<character>& players);

//ввод - вывод info персонажей
void load_characters(std::vector<character>& characters);
void save_characters(std::vector<character>& characters);
void print_character(character &person);

//состояние игры
bool game_over(std::vector<character>& players);

//состояния персонажей
bool out_of_map(vec& position);
bool is_alive(character& person);

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

    //Ход игры:
    while(!game_over(players)){
        //отрисовка карты:
        map_painter(players);

        for(int i = 0; i < players.size(); i++){
            std::cout << "Ход игрока " << players[i].name << std::endl;
            print_character(players[i]);

            int pos_x = players[i].position.x;
            int pos_y = players[i].position.y;

            if(players[i].flag){
                std::string direction;
                std::cout << "Ваш ход 'w - s - a - d'/(load/save): ";
                std::cin >> direction;

                if(direction == "load") {
                    load_characters(players);
                } else if (direction == "save"){
                    save_characters(players);
                } else {
                    if(direction == "w")
                        players[i].position.x--;
                    else if(direction == "s")
                        players[i].position.x++;
                    else if(direction == "a")
                        players[i].position.y--;
                    else if(direction == "d")
                        players[i].position.y++;
                }
                // проверить не вышел ли за границы - check boundaries
                if (out_of_map(players[i].position)){
                    std::cout << "Out of bounds." << std::endl;
                    //вернуться обратно
                    players[i].position.x = pos_x;
                    players[i].position.y = pos_y;
                    continue;
                } else {
                    for(int next = 0; next <  players.size(); next++){
                        if(i == next) continue;
                        //проверить не занята ли новая позиция другим игроком
                        if(players[i].position.x == players[next].position.x
                           && players[i].position.y == players[next].position.y){
                            //если занята игроком с другим флагом - то батл
                            if((players[i].flag != players[next].flag) && is_alive(players[next])){
                                take_damage(players[next], players[i].damage);
                            } else {
                                //если занята игроком того же флага, то вернуться
                                //на изначальную позицию и завершить ход
                                players[i].position.x = pos_x;
                                players[i].position.y = pos_y;
                                break;
                            }
                        }
                    }
                }
            } else {
                move_enemy(players[i]);
                //тоже самое от if(bound_checker())
                if (out_of_map(players[i].position)){
                    std::cout << "Out of bounds." << std::endl;
                    //вернуться обратно
                    players[i].position.x = pos_x;
                    players[i].position.y = pos_y;
                    continue;
                } else {
                    for(int next = 0; next <  players.size();next++){
                        if(i == next) continue;
                        //проверить не занята ли новая позиция другим игроком
                        if((players[i].position.x == players[next].position.x)
                           && (players[i].position.y == players[next].position.y)){
                            //если занята игроком с другим флагом - то батл
                            if((players[i].flag != players[next].flag) && is_alive(players[next])){
                                take_damage(players[next], players[i].damage);
                            } else {
                                //если занята игроком того же флага, то вернуться
                                //на изначальную позицию и завершить ход
                                players[i].position.x = pos_x;
                                players[i].position.y = pos_y;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
};


character hero_init(){
    character person;
    std::cout << "Enter the name: ";
    std::cin >> person.name;
    std::cout << "Health: ";
    std::cin >> person.health;
    std::cout << "Armor: ";
    std::cin >> person.armor;
    std::cout << "Damage: ";
    std::cin >> person.damage;

    person.position.x = (std::rand() % 19);
    person.position.y = (std::rand() % 19);

    return person;
};

character enemy_init(int i){
    character person;
    person.flag = false;
    person.name = "Enemy " + std::to_string(i);
    person.health = (std::rand() % 100) + 50;
    person.armor = (std::rand() % 50);
    person.damage = (std::rand() % 15) + 15;
    person.position.x = (std::rand() % 19);
    person.position.y = (std::rand() % 19);

    return person;
};

void take_damage(character& person, int damage) {
    person.armor -= damage;
    if (person.armor < 0) {
        person.health += person.armor;
        person.armor = 0;
    }
};

/*
void move_hero(character& person){
  std::string direction;
  std::cout << "Ваш ход 'w -s - a - d'/(load/save): ";
  std::cin >> direction;
  
  if(direction == "load") {//strcmp()
    load_characters(person);
  } else if (direction == "save"){
    save_characters(person);
  } else {
    if(direction == "w") 
      person.position.x--;
    else if(direction == "s")
      person.position.x++;
    else if(direction == "a") 
      person.position.y--;
    else if(direction == "d")
      person.position.y++;          
  }
};
*/

void move_enemy(character& person){
    int direction = ((std::rand() % 4) + 1);
    switch(direction){
        case UP: person.position.x--; break;
        case DOWN: person.position.x++; break;
        case LEFT: person.position.y--; break;
        case RIGHT: person.position.y++; break;
    }
};

void print_character(character &person) {
    std::cout << std::endl;
    std::cout << "Name: " << person.name << std::endl;
    std::cout << "Health: " << person.health << std::endl;
    std::cout << "Armor: " << person.armor << std::endl;
    std::cout << "Damage: " << person.damage << std::endl;
    std::cout << "Position: " << person.position.x + 1;
    std::cout << ", " << person.position.y + 1 << std::endl;
};

void map_painter(std::vector <character>& players){
    int map[MAP_SIZE][MAP_SIZE] = {0}; //game field

    for(auto & player : players){
        if(is_alive(player)){
            if(player.flag){
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
};

void load_characters(std::vector <character>& characters){
    std::ifstream file("save.bin", std::ios::binary);
    for (int i = 0; i < 6; ++i){
        file.read((char*)&characters[i].flag, sizeof(characters[i].flag));

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
};

void save_characters(std::vector <character>& characters){
    std::ofstream file("save.bin", std::ios::binary);
    for (int i = 0; i < 6; ++i) {
        file.write((char*)&(characters[i].flag), sizeof(characters[i].flag));

        int name_len = characters[i].name.length();
        file.write((char*)&name_len, sizeof(name_len));
        file.write(characters[i].name.c_str(), name_len);

        file.write((char*)&(characters[i].health), sizeof(characters[i].health));
        file.write((char*)&(characters[i].armor), sizeof(characters[i].armor));
        file.write((char*)&(characters[i].damage), sizeof(characters[i].damage));
        file.write((char*)&(characters[i].position.x), sizeof(characters[i].position.x));
        file.write((char*)&(characters[i].position.y), sizeof(characters[i].position.y));
    }
};


bool game_over(std::vector <character>& players){
    int enemy_defeated_counter = 0;// возможно должна быть глобальным счетчиком - инфо состояние игры.

    for(auto & player : players){
        if(!is_alive(player)){
            if(player.flag) {
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
};

bool out_of_map(vec& position){
    if(position.x >= 20 || position.x < 0
       || position.y >= 20 || position.y < 0 ) return true;
    else return false;
};

bool is_alive(character& person){
    if (person.health > 0) return true;
    return false;
}


