#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>


int width = 1000;
int height = 500;
int FPS = 30;

int screen_cells = 21;      // number of cells displayed on screen
int offscreen_cells = 20;   // number of cells displayed outside the screen (left + right)
int gap_size = 5;           // gap between cells in pixels

float cell_size = (width-gap_size*(screen_cells+1))/(screen_cells);
int total_cells = screen_cells + offscreen_cells;


/*
Rule formatting:
    Each row is a different rule/state
    In each row the elements correspond to:

        0th value: rule number 0,1,2,..

        Consequent 3-tuples correspond to each possible symbol (0,1 in 2-symbol case):

        1st value: if cell is 0, specifies what to write
        2nd value: if cell is 0, specifies where to move the tape. 0 - left, 1 - right
        3rd value: if cell is 0, specifies the next rule index -1,0,1,2,.. where -1 is the halt state

        4th value: if cell is 1, specifies what to write
        5th value: if cell is 1, specifies where to move the tape. 0 - left, 1 - right
        6th value: if cell is 1, specifies the next rule index -1,0,1,2,.. where -1 is the halt state
        ...

 */

// number of rows -- number of rules/states
// number of columns -- 1+3*(number of symbols)
int Rules[4][7] = {{0,      1, 1, 1,        1, 0, 1},
                   {1,      1, 0, 0,        0, 0, 2},
                   {2,      1, 1, -1,       1, 0, 3},
                   {3,      1, 1, 3,        0, 1, 0}};

std::vector<sf::Text> create_tape_text(int x_start, int configuration[])
{
    /*
    Returns a vector of Text objects with properties (excluding font) corresponding to the configuration on the tape

        Args:
            x_start - x-position of the beginning of tape
            configuration - array holding the configuration state of the tape

        Returns tape_text - vector containing the tape configuration specified with sf::Text objects
    */


    std::vector<sf::Text> tape_text;
    int y_coord = height/3 - cell_size/8;    // text y-coordinate

    sf::Text text;
    text.setCharacterSize(cell_size);
    text.setStyle(sf::Text::Bold);
    text.setColor(sf::Color(113, 32, 193));

    for (int cell=0; cell<total_cells; cell++)
    {
        text.setString(std::to_string(configuration[cell]));
        text.setPosition(cell_size/4 + x_start+gap_size+cell*(cell_size+gap_size), y_coord);
        tape_text.push_back(text);
    }

    return tape_text;
}

std::vector<sf::RectangleShape> create_tape(int x_start, int configuration[])
{
    /*
    Returns a vector of rectangle shapes making up the Turing machine tape

        Args:
            x_start - x-position of the beginning of tape
            configuration - array holding the configuration state of the tape

        Returns:
            tape - vector containing rectangle shapes making up the Turing machine tape
    */


    std::vector<sf::RectangleShape> tape;
    int y_coord = height/3;      // y-coordinate of the tape

    sf::RectangleShape rect;
    rect.setFillColor(sf::Color(113, 32, 193));
    rect.setSize(sf::Vector2f(cell_size, cell_size));

    for (int cell=0; cell<total_cells; cell++)
    {
        rect.setFillColor(sf::Color::White);
        rect.setPosition(x_start+gap_size+cell*(cell_size+gap_size), y_coord);
        tape.push_back(rect);
    }

    return tape;
}


sf::VertexArray head(int index)
{
    /*
    Returns array of vertices of the head of the Turing machine

        Args:
            index - index of the cell on the tape at which the head is positioned.
                    0th index is the middle of the screen, negatives on the left, positives on the right

        Returns:
            triangle - vertex array of the head (a triangle) of the Turing machine
    */

    int y_coord = height/3 + cell_size;    // y-coordinate of the tip of the triangle

    sf::VertexArray triangle(sf::Triangles, 3);
    triangle[0].position = sf::Vector2f(index*(cell_size+gap_size) + cell_size/2+gap_size, y_coord+gap_size);
    triangle[1].position = sf::Vector2f(index*(cell_size+gap_size) + cell_size/2+gap_size-20, y_coord+gap_size+20);
    triangle[2].position = sf::Vector2f(index*(cell_size+gap_size) + cell_size/2+gap_size+20, y_coord+gap_size+20);

    triangle[0].color = sf::Color(30, 188, 162);
    triangle[1].color = sf::Color(30, 188, 162);
    triangle[2].color = sf::Color(30, 188, 162);

    return triangle;
}


int move_tape(std::string direction, int x_1, int configuration[], sf::RenderWindow& window, std::vector<sf::RectangleShape> tape, std::vector<sf::Text> tape_text, sf::VertexArray triangle)
{
    /*
    Moves the tape to either left or right, as specified by 'direction'.
    Returns the final position of the tape.
    Also draws the tape with the head during the configuration-transition period

        Args:
            x_1 - initial x-position of the beginning of the tape
            configuration - array holding the configuration state of the tape
            window - SFML window object
            tape - vector of rectangle shapes making up the tape
            tape_text - vector of tape configuration specified with sf::Text objects
            triangle - vertex array of the head
            step - current step of the program

        Returns:
            x_2 - final x-position of the beginning of the tape
    */

    int speed, x_2;
    bool condition;

    if (direction == "left")
        {
          speed = -5;
          x_2 = x_1 - cell_size - gap_size;
          condition = x_1 > x_2;
        }

    if (direction == "right")
        {
          speed = 5;
          x_2 = x_1 + cell_size + gap_size;
          condition = x_1 < x_2;
        }

    // additionally, write the symbols on the cells
    sf::Text text;
    sf::Font font;

    if (!font.loadFromFile("arial.ttf"))
        std::cout << "Error loading font" << std::endl;


    while (condition)      // incrementally move the tape and display
    {
        if (direction == "left"){condition = x_1 > x_2;}
        if (direction == "right"){condition = x_1 < x_2;}

        tape = create_tape(x_1, configuration);
        tape_text = create_tape_text(x_1, configuration);
        x_1 += speed;


        window.clear(sf::Color::Black);

        // draw the tape
        for (int j=0;j<total_cells;j++)
        {
            window.draw(tape[j]);

            text = tape_text[j];
            text.setFont(font);
            window.draw(text);
        }

        window.draw(triangle);

        window.display();
        window.setFramerateLimit(FPS);
    }

    return x_2;
}



int main()
{
    // Request a 24-bits depth buffer when creating the window
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 24;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(width, height), "SFML", sf::Style::Default, contextSettings);
    sf::Clock clock;

    int x_start = (-gap_size - cell_size)*offscreen_cells/2;    // start x-position of the tape


    int configuration[total_cells] = {0};    // configuration state array of the tape
    sf::VertexArray triangle = head((screen_cells-1)/2);    // pointer/head vertex array

    // vector to store rectangles for the tape (for drawing purposes)
    std::vector<sf::RectangleShape> tape = create_tape(x_start, configuration);

    // vector to store the configuration of the tape (for drawing purposes)
    std::vector<sf::Text> tape_text = create_tape_text(x_start, configuration);

    int head_index = (total_cells-1)/2;     // index of the cell that the head is initially pointing at
    int rule_index = 0;                     // initial rule
    int new_rule_index;                     // update-variable
    int new_head_index;
    int new_configuration[total_cells];     // update-array

    int step = 0;                           // step counter


    // Start the game loop
    bool running = true;
    while (running)
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                running = false;
        }

        if (rule_index != -1)   // if not halted
        {

            step += 1;
            std::cout << "Step " << step << std::endl;

            if (configuration[head_index] == 0)     // if current cell has a 0
            {
                std::copy(configuration, configuration+total_cells, new_configuration);
                new_configuration[head_index] = Rules[rule_index][1];

                if (Rules[rule_index][2] == 0)      // if satisfied, move tape left
                {
                    x_start = move_tape("left", x_start, new_configuration, window, tape, tape_text, triangle);
                    new_head_index = head_index + 1;
                }
                if (Rules[rule_index][2] == 1)      // if satisfied, move tape right
                {
                    x_start = move_tape("right", x_start, new_configuration, window, tape, tape_text, triangle);
                    new_head_index = head_index - 1;
                }

                new_rule_index = Rules[rule_index][3];  //  retrieve the next rule-state
            }


            if (configuration[head_index] == 1)     // if a current cell has a 1
            {
                std::copy(configuration, configuration+total_cells, new_configuration);
                new_configuration[head_index] = Rules[rule_index][4];

                if (Rules[rule_index][5] == 0)
                {
                    x_start = move_tape("left", x_start, new_configuration, window, tape, tape_text, triangle);
                    new_head_index = head_index + 1;
                }
                if (Rules[rule_index][5] == 1)
                {
                    x_start = move_tape("right", x_start, new_configuration, window, tape, tape_text, triangle);
                    new_head_index = head_index - 1;
                }

                new_rule_index = Rules[rule_index][6];
            }

            // update
            head_index = new_head_index;
            rule_index = new_rule_index;
            std::copy(new_configuration, new_configuration+total_cells, configuration);
        }

        if (rule_index == -1)   // if halted, then compute the final configuration
        {

            int ix_start, ix_end;
            int j = 0;
            int element = configuration[j];

            while (element == 0)
            {
                j += 1;
                element = configuration[j];
            }
            ix_start = j;

            j = total_cells - 1;
            element = configuration[j];

            while (element == 0)
            {
                j -= 1;
                element = configuration[j];
            }
            ix_end = j;

            std::cout << "Final configuration: " << std::endl;
            for (int i=ix_start-3; i<ix_end+3; i++)
                std::cout << configuration[i] << " ";

            sf::Time delay = sf::milliseconds(5000);
            sf::sleep(delay);
            break;

        }
    }


    return 0;
}
