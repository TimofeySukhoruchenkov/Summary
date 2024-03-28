#include <iostream>
#include <vector>
#include <string>

const int GRID_SIZE = 10; // размер игрового поля
const char EMPTY = '.';
const char PLAYER1_VIRUS = 'x';
const char PLAYER2_VIRUS = 'o';
const char PLAYER1_FORTRESS = '#';
const char PLAYER2_FORTRESS = '@';
using namespace std;

enum class errors {outsideMap, yourSquare, enemyFortress,
        yourFortressSquare, emptyLocationAround, NoError, WrongFirstPick}; //ошибки, которые могут быть во время хода

class TGameLogic {
private:
    char player[2][2]; //массив, который будет хранить константы, являющиеся знаками вирусов и крепостей

protected:
    pair<char, int> grid[GRID_SIZE][GRID_SIZE]; //игровое поле

public:
    TGameLogic() {
        for (auto & i : grid) {
            for (auto & j : i) {
                j.first = EMPTY;
                j.second = 0;
            }
        }

        player[0][0] = PLAYER1_VIRUS;
        player[0][1] = PLAYER1_FORTRESS;
        player[1][0] = PLAYER2_VIRUS;
        player[1][1] = PLAYER2_FORTRESS;
    }

    errors CheckMove(int x, int y, int playerNumber) const { //нахождение ошибки во время хода
        if (x >= GRID_SIZE || x < 0 || y >= GRID_SIZE || y < 0) {
            return errors::outsideMap; //координаты за игровым полем
        } else if (grid[x][y].first == player[playerNumber][0]) {
            return errors::yourSquare; //на этой клетке и так есть свой вирус
        } else if (grid[x][y].first == player[playerNumber][1]) {
            return errors::yourFortressSquare; //на этой клетке своя крепость
        } else if (grid[x][y].first == player[(playerNumber + 1) % 2][1]) {
            return errors::enemyFortress; //на этой клетке вражеская крепость
        } else {
            int flag = 0;
            for (int x2 = x - 1; x2 <= x + 1; x2++) {
                if (x2 < GRID_SIZE && x2 >= 0) {
                    for (int y2 = y - 1; y2 <= y + 1; y2++) {
                        if (y2 < GRID_SIZE && y2 >= 0) {
                            if (grid[x2][y2].first == player[playerNumber][0]) {
                                flag = 1;
                                break;
                            } else if (grid[x2][y2].first == player[playerNumber][1]) {
                                if (grid[x2][y2].second == 1) {
                                    flag = 1;
                                    break;
                                }
                            }
                        }
                    }
                    if (flag == 1) {
                        break;
                    }
                }
            }
            if (flag == 0) {
                return errors::emptyLocationAround; //вокруг этой клетки нет живых дружественных структур
            } else {
                return errors::NoError; //ошибок нет
            }

        }
    }
    void CorrectCellProcessing(int x, int y, int playerNumber) {//обработка хода в случае его верности
        if (grid[x][y].first == EMPTY) { //обработка в случае, когда клетка свободна
            grid[x][y].first = player[playerNumber][0]; // ставим вирус в эту клетку
            grid[x][y].second = 1;
            UpdatePoint(x, y, playerNumber); // обновляем живые крепости вокруг
        } else { //обработка в случае, когда в клетке вражеский вирус
            grid[x][y].first = player[playerNumber][1]; //ставим свою крепость в эту клетку
            grid[x][y].second = 1;
            UpdatePoint(x, y, playerNumber); //обновляем живые крепости вокруг
            DeleteFortressCell(x, y, (playerNumber + 1) % 2); //удаляем вражескую крепость и обнуляем все вражеские крепости вокруг
            FullUpdate((playerNumber + 1) % 2); //обновляем состояние вражеских крепостей, которые были связаны с этой крепостью
            grid[x][y].second = 1;
        }
    }

private:

    void UpdatePoint(int x, int y, int playerNumber) { //оживление всех своих крепостей вокруг в случае, когда в эту клетку ставят союзную структуру
        for (int x2 = x - 1; x2 <= x + 1; x2++) {
            if (x2 < GRID_SIZE && x2 >= 0) {
                for (int y2 = y - 1; y2 <= y + 1; y2++) {
                    if (y2 < GRID_SIZE && y2 >= 0) {
                        if (grid[x2][y2].first == player[playerNumber][1]) {
                            if (grid[x2][y2].second == 0) {
                                grid[x2][y2].second = 1;
                                UpdatePoint(x2, y2, playerNumber);
                            }
                        }
                    }
                }
            }
        }
    }

    void FullUpdate(int playerNumber) { //обновление всех своих крепостей после удаления какой-либо союзной крепости
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                UpdateAfterDelete(i, j, playerNumber);
            }
        }
    }

    void UpdateAfterDelete(int x, int y, int playerNumber) { //обновление любой крепости после удаления какой-либо союзной крепости
        int flag = 0;
        for (int x2 = x - 1; x2 <= x + 1; x2++) {
            if (x2 < GRID_SIZE && x2 >= 0) {
                for (int y2 = y - 1; y2 <= y + 1; y2++) {
                    if (y2 < GRID_SIZE && y2 >= 0) {
                        if ((grid[x2][y2].first == player[playerNumber][0] || grid[x2][y2].first == player[playerNumber][1]) && grid[x2][y2].second == 1) {
                            grid[x][y].second = 1;
                            flag = 1;
                            break;
                        }
                    }
                }
                if (flag == 1) {
                    break;
                }
            }
        }

        if (flag == 1) {
            for (int x2 = x - 1; x2 <= x + 1; x2++) {
                if (x2 < GRID_SIZE && x2 >= 0) {
                    for (int y2 = y - 1; y2 <= y + 1; y2++) {
                        if (y2 < GRID_SIZE && y2 >= 0) {
                            if (grid[x2][y2].first == player[playerNumber][1] && grid[x2][y2].second == 0) {
                                UpdateAfterDelete(x2, y2, playerNumber);
                            }
                        }
                    }
                }
            }
        }
    }

    void DeleteFortressCell(int x, int y, int playerNumber) { //умертвение всех своих крепостей вокруг удаленной крепости
        grid[x][y].second = 0;
        for (int x2 = x - 1; x2 <= x + 1; x2++) {
            if (x2 < GRID_SIZE && x2 >= 0) {
                for (int y2 = y - 1; y2 <= y + 1; y2++) {
                    if (y2 < GRID_SIZE && y2 >= 0) {
                        if (grid[x2][y2].first == player[playerNumber][1] && grid[x2][y2].second == 1) {
                            DeleteFortressCell(x2, y2, playerNumber);
                        }
                    }
                }
            }
        }
    }


};




class TGameEngine : private TGameLogic {
public:

    TGameEngine()
        : TGameLogic()
        {
        }

    void ReadFromStream(istream& stream) { //обработка вводимых данных
        int x, y;
        cout << "Game Start:" << endl;
        stream >> x >> y;
        errors error;
        while ((error = FirstPickPlayer(x, y)) != errors::NoError) {
            ErrorsHandler(error);
            stream >> x >> y;
        }
        grid[0][0].first = PLAYER1_VIRUS;
        grid[0][0].second = 1;
        Out();
        for (int i = 0; i < 2; i++) {
            stream >> x >> y;
            while ((error = CheckMove(x - 1, y - 1, 0)) != errors::NoError) {
                ErrorsHandler(error);
                stream >> x >> y;
            }
            CorrectCellProcessing(x - 1, y - 1, 0);
            Out();
        }

        stream >> x >> y;
        while ((error = SecondPickPlayer(x, y)) != errors::NoError) {
            ErrorsHandler(error);
            stream >> x >> y;
        }
        grid[9][9].first = PLAYER2_VIRUS;
        grid[9][9].second = 1;
        Out();

        for (int i = 0; i < 2; i++) {
            stream >> x >> y;
            while ((error = CheckMove(x - 1, y - 1, 1)) != errors::NoError) {
                ErrorsHandler(error);
                stream >> x >> y;
            }
            CorrectCellProcessing(x - 1, y - 1, 1);
            Out();
        }

        int playerNumber = 0;

        while (Loser(playerNumber)) {
            for (int i = 0; i < 3; i++) {
                stream >> x >> y;
                while ((error = CheckMove(x - 1, y - 1, playerNumber)) != errors::NoError) {
                    ErrorsHandler(error);
                    stream >> x >> y;
                }
                CorrectCellProcessing(x - 1, y - 1, playerNumber);
                Out();
            }
            playerNumber = (playerNumber + 1) % 2;
        }
        cout << "Player: " << playerNumber + 1 << " lost" << endl;
    }

private:

    errors FirstPickPlayer(int x, int y) const { //обработка первого хода первого игрока на наличие ошибки
        if (x != 1 || y != 1){
            return errors::WrongFirstPick;
        }
        return errors::NoError;
    }

    errors SecondPickPlayer(int x, int y) const { //обработка первого хода второго игрока на наличие ошибки
        if (x != 10 || y != 10){
            return errors::WrongFirstPick;
        }
        return errors::NoError;
    }

    bool Loser(int playerNumber) const { //проверка на возможность игрока выполнить три хода (то есть проверка на поражение)
        int sum = 0;
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (CheckMove(i, j, playerNumber) == errors::NoError) {
                    sum++;
                }
            }
        }
        if (sum < 3) {
            return false;
        }
        return true;
    }

    void ErrorsHandler(errors error) const { //вывод на экран в зависимости от ошибки
        if (error == errors::outsideMap) {
            cout << "Coordinates outside the map" << endl;
        } else if (error == errors::yourSquare || error == errors::yourFortressSquare) {
            cout << "Already your cell" << endl;
        } else if (error == errors::enemyFortress) {
            cout << "Its enemy fortress" << endl;
        } else if (error == errors::emptyLocationAround) {
            cout << "No living friendly cell around" << endl;
        } else if (error == errors::WrongFirstPick) {
            cout << "Your first move must be" << endl <<"1 1 for the first player"<< endl
                 << "10 10 for the second player" << endl;
        }
    }

    void Out() const { //вывод игрового поля на экран
        for (auto & i : grid) {
            for (auto & j : i) {
                cout << j.first << "-" << j.second << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

};



int main () {
    TGameEngine T;
    T.ReadFromStream(std::cin);
}