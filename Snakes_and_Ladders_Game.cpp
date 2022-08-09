#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

double MRKV[101][101] = { 0 }; // Main Markov matrix that contain the probabilities of the future state

void Make_MRKV() // The function that initialize the Markov matrix to the probability values -- WITHOUT -- snakes or ladders
{
	for (int i = 0; i < 101; i++)
		if (i >= 95)
			for (int j = i; j < 101; j++)
				if (i == j)
					MRKV[i][j] = (6 - (100 - float(i))) / 6;
				else MRKV[i][j] = 1.0 / 6.0;
		else
			for (int j = i + 1; j <= i + 6; j++)
				MRKV[i][j] = 1.0 / 6.0;
}


int L_Ladder[8] = { 9, 10, 16, 20, 21, 22, 37, 56 }; // Lengths of ladders that can be chosen randomly for a ladder, aka: Each ladder will choose randomly from these values
int L_Snake[8] = { 3, 4, 10, 20, 21, 38, 43, 63 }; // Lengths of snakes that can be chosen randomly for a ladder, aka: Each snake will choose randomly from these values

int L_begin[9] = { 1, 4, 9, 21, 28, 36, 51, 71, 80 };        // fixed beginnings for ladders
int S_begin[9] = { 98, 95, 93, 87, 64, 62, 56, 49, 48 };    // fixed beginnings for snakes


struct Ladder { // a struct defining the data type ladder and identifying its data members
	int Beginning;
	int Length;
	int End;                        // The End of the ladder must equal its Beginning plus its Length
};

struct Snake { // a struct defining the data type snake and identifying its data members
	int Beginning;
	int Length;
	int End;                          // The End of the snake must equal its Beginning minus its Length
};


void Initialize_length_beginning_Ladder_Snake(Ladder Ladder_arr[9], Snake Snake_arr[9]) // Give the ladders and the snakes their initial Lengths and beginning values
{
	for (int i = 0; i < 9; i++)
	{
		Ladder_arr[i].Beginning = L_begin[i];
		Ladder_arr[i].Length = L_Ladder[rand() % 8] ;
		Ladder_arr[i].End = Ladder_arr[i].Beginning + Ladder_arr[i].Length;

		Snake_arr[i].Beginning = S_begin[i];
		Snake_arr[i].Length = L_Snake[rand() % 8] ;
		Snake_arr[i].End = Snake_arr[i].Beginning - Snake_arr[i].Length;
	}
}

void Check_Errors(Ladder check_ladder[9], Snake check_snake[9])
{
	for (int i = 0; i < 9; i++)
	{
		int j = 0;
		int rejected_length[8] = { 0 };         // A list of rejected lengths of a certain Ladder so that no new assigned length is the same as the refused ones
		bool Condition = false;
		int random_new_length;

		while (check_ladder[i].End > 100)                          // The End of the ladder must not exceed square 100
		{
			rejected_length[j] = check_ladder[i].Length;

			do {
				Condition = false;
				random_new_length = L_Ladder[rand() % 8];                                // Generate new length
				for (int k = 0; k <= j; k++)                                            // Then compare this length with the list of rejected lengths
					if (random_new_length == rejected_length[k]) Condition = true;     // If the new length is ok it exits the small loop and recheck the end condition again
			}while(Condition);

			check_ladder[i].Length = random_new_length;
			check_ladder[i].End = check_ladder[i].Beginning + check_ladder[i].Length;
			j++;
		}

		  j = 0;                                                          // reinitialize both j and the rejected lengths list and the condition
		  Condition = false;
		  for(int m = 0; m <8; m++) rejected_length[m] = 0;

		while (check_snake[i].End < 1)                          // The End of the snake must not be less than square 1
		{
			rejected_length[j] = check_snake[i].Length;       // The rejected lengths are collected in one array so that they are not chosen twice

			do {
				Condition = false;
				 random_new_length = L_Snake[rand() % 8];
				for (int k = 0; k <= j; k++)
					if (random_new_length == rejected_length[k]) Condition = false;               // if the new chosen length was the same as the rejected ones a new length must be assigned
			} while (Condition);

			check_snake[i].Length = random_new_length;
			check_snake[i].End = check_snake[i].Beginning - check_snake[i].Length;
			j++;
		}
	}

	for (int i = 0; i < 9; i++)              // To make sure that the end of the ladders and snakes don't lie on the beginning of another snake or ladder
	{
		for (int j = 0; j < 9; j++)         // make the check for ladders when detecting an error square we shift the end of the snake or ladder right or left
		{                   			       //that square by one square untill liying on square without errors
			while (check_ladder[i].End == L_begin[j] || check_ladder[i].End == S_begin[j])
			{
				if (check_ladder[i].End < 100) { check_ladder[i].Length++; check_ladder[i].End = check_ladder[i].Beginning + check_ladder[i].Length; }
				else
				{
					while (check_ladder[i].End == L_begin[j] || check_ladder[i].End == S_begin[j])
					{
						check_ladder[i].Length--;
						check_ladder[i].End = check_ladder[i].Beginning + check_ladder[i].Length;
					}
					break;
				}
			}
		}
		for (int j = 0; j < 9; j++) // make the check for snakes
		{
			while (check_snake[i].End == L_begin[j] || check_snake[i].End == S_begin[j])
			{
				if (check_snake[i].End > 1) { check_snake[i].Length++; check_snake[i].End = check_snake[i].Beginning - check_snake[i].Length; }
				else
				{
					while (check_snake[i].End == L_begin[j] || check_snake[i].End == S_begin[j])
					{
						check_snake[i].Length--;
						check_snake[i].End = check_snake[i].Beginning - check_snake[i].Length;
					}
					break;
				}
			}
		}
	}

}


struct Player { // The Player will have a current position which changes with each turn and a name
	string name;
	int position = 0;
	int Column[101] = { 0 };                    // The column of the Markov matrix that contains the probabilities of the next position for the current player

	void Display_Message(Snake snakes[9], Ladder ladders[9], bool& condition) // The messages displayed  and actions taken during the gameplay
	{
		cout << "It's Player " << name << " turn, PRESS ENTER Button to roll the dice!!!";
		while(true){
			char Play_button;
			Play_button = cin.get();
			if (Play_button == '\n')      break;
			else
				cout << "Please Press ONLY the ENTER BUTTON to roll the dice!!!";
		}

		bool Disp_Pos = true;        // a variable to control the position displayed

		for (int i = 0; i < 9; i++)
		{
			if (position == snakes[i].Beginning)
			{
				position = snakes[i].End;
				cout << "\nOOPS!!! player " << name << " has slipped over a snake and he is now at position " << position << endl;
				Disp_Pos = false;
				break;
			}

			if (position == ladders[i].Beginning)
			{
				position = ladders[i].End;
				cout << "\nYAY!! player " << name << " has climbed a ladder and he is now at position " << position << endl;
				Disp_Pos = false;
				break;
			}
		}

		if (position >= 100)
		{
			cout << "\n\n\t\tCONGRAULATIONS!!!! Player " << name << " YOU HAVE WON!!!!\n\n";
			condition = true;
			Disp_Pos = false;
		}

		if(Disp_Pos)
			cout << "\nPlayer " << name << " is now at position " << position << endl;
	}
};


void Assign_P_next_positions(Player& player_n)                     // This function takes the current positon of the player and returns a column from the Markov matrix
{                                                                // containing the probabilty weights of the next position which are represented by the row number in this column
	for (int i = 0; i < 101; i++)
		player_n.Column[i] = ceil( 6 * MRKV[player_n.position][i]);             // The elements are multiplied by six so that they are ready for the function that generates the
                                                                               //  random number according to the probablilty weight

}

int Get_Next_Position(int x[105])
{
	int prop[10] = { 0 }, l = 0;
	for (int i = 0; i < 101; i++)
	{
		for (int j = 0; j < (x[i]); j++)
		{
			prop[l] = i;
			l++;
		}
	}
	int a = (rand() % 6) + 1;
	return prop[a - 1];
}

int main()
{
	srand( time(NULL) );
	Make_MRKV();

	Ladder ladders[9]; Snake snakes[9];

	Initialize_length_beginning_Ladder_Snake(ladders, snakes);

	Check_Errors(ladders, snakes);
    cout << "For snake : S = ( Beginning , End ) \t \t \t For Ladder : L = ( Beginning , End )\n";
   cout << endl << "\t\t\t\t****************************\t\t" << endl;
	for (int i = 0; i < 9; i++)
	{
		cout << "S" << i + 1 <<" = ( "<<snakes[i].Beginning<<" , "<< snakes[i].End<<" )\t";
        if(!((i+1)%4)) cout<<endl;
	}

	cout << endl << "\t\t\t\t****************************\t\t" << endl;
		for (int i = 0; i < 9; i++)
	{
		cout << "L" << i + 1 <<" = ( "<<ladders[i].Beginning<<" , "<< ladders[i].End<<" )\t";
        if(!((i+1)%4)) cout<<endl;
	}
		cout << endl << "\t\t\t\t****************************\t\t" << endl;


	Player player_1, player_2;                                     // Instantiating two player objects

	cout << "Enter the name of player 1: "; cin >> player_1.name; // Ask and Get the names of both players
	cout << "Enter the name of player 2: "; cin >> player_2.name;
	cin.ignore();                    // ignore the Enter pressed after writing the Names so that

	bool Conditon = false;

	                                               // The Gameplay CODE
	while (true)
	{
		Assign_P_next_positions(player_1);                  // Prepare the probabilities of next position
		Assign_P_next_positions(player_2);

		player_1.position = Get_Next_Position(player_1.Column);          // Update the position for each player
		player_2.position = Get_Next_Position(player_2.Column);

		player_1.Display_Message(snakes, ladders, Conditon);            // Display the messages during the gameplay for each player
		cout << endl;
		if (Conditon) exit(0);                                          // When the Player wins the program Exits

		player_2.Display_Message(snakes, ladders, Conditon);
		cout << endl;
		if (Conditon) exit(0);
	}
}
