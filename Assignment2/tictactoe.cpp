#include <bits/stdc++.h>

using namespace std;

int main(){
    
    int board[3][3];

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            board[i][j] = 0;
        }
    }

    int player = 1;
    int row, col;
    int work[] = {0, 1, 10};
    int sum = 0;
    int i = 0;
    int j = 0;
    int pl = 0;
    bool check = 0;

    while(true){
        
        cout << "Player " << player << "'s turn\n";
        cout << "Enter row number\n";
        cin >> row;
        cout << "Enter column number\n";
        cin >> col;
        --row;
        --col;
        if(row < 0 || row > 2 || col < 0 || col > 2 || board[row][col] != 0){
            cout << "Wrong input, please try again\n";
            continue;
        }
        player = 3 - player;
        board[row][col] = work[player];
        
        //printing
        for(i = 0; i < 3; i++){
             for(j = 0; j < 3; j++){
                 if(board[i][j] == 0){ 
                     cout << " ";
                 }
                 else if(board[i][j] == 1){ 
                     cout << "X";
                 }
                 else cout << "O";
             }
             cout << "\n";
         }

        for(i = 0; i < 3; i++){
            sum = 0;
            for(j = 0; j < 3; j++){
                sum += board[i][j];
            }
            for(pl = 1; pl < 3; pl++){
                if(sum == work[pl]*3){
                    cout << "Player " << pl << "wins\n";
                    return 0;
                }
            }
        }

        for(j = 0; j < 3; j++){
            sum = 0;
            for(i = 0; i < 3; i++){
                sum += board[i][j];
            }
            for(pl = 1; pl < 3; pl++){
                if(sum == work[pl]*3){
                    cout << "Player " << pl << "wins\n";
                    return 0;
                }
            }
        }

        sum = 0;
        for(i = 0; i < 3; i++){
            sum += board[i][i];
        }

        for(pl = 1; pl < 3; pl++){
            if(sum == 3*work[pl]){
                cout << "Player " << pl << "wins\n";
                return 0;
            }
        }

        sum = 0;
        for(i = 0; i < 3; i++){
            sum += board[i][2-i];
        }

        for(pl = 1; pl < 3; pl++){
            if(sum == work[pl]*3){
                cout << "Player " << pl << "wins\n";
                return 0;
            }
        }

        check = true;

        for(i = 0; i < 3; i++){
            for(j = 0; j < 3; j++){
                check = check && (board[i][j] != 0);
            }
        }

        if(check){
            cout << "Tie\n";
            return 0;
        }
    }
}
