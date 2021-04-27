#include <iostream>
#include <string>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Coord
{
public:
    float x,y;
    Coord();
    Coord(float,float);
};

class Sandbag
{
    sf::RenderWindow* window; //SFML window object
    sf::Texture texture; //Sandbag texture
    sf::Sprite sprite; //Sandbag sprite
    Coord pos; //Sandbag position
public:

    /*
    @brief
        Initializes a sandbag object
    @params
        window: SFML window object
        texturePath: This is the path to the sandbag texture file
        pos: Sandbag position
    */
    void init(sf::RenderWindow *window, std::string texturePath, Coord pos);

    //Returns sandbag position
    Coord getPosition();

    //Draws the sandbag sprite
    void paint();
};

class Barrel
{
    sf::RenderWindow* window; //SFML window object
    sf::Texture texture; //Barrel texture
    sf::Sprite sprite; //Barrel sprite
    Coord pos; //Barrel position
public:

    /*
    @brief
        Initializes a barrel object
    @params
        window: SFML window object
        texturePath: This is the path to the barrel texture file
        pos: Barrel position
    */
    void init(sf::RenderWindow *window, std::string texturePath, Coord pos);

    //Returns barrel position
    Coord getPosition();

    //Draws the barrel sprite
    void paint();
};

class Player
{
    sf::Texture textures[14]; //Player texture array (one element per soldier state)
    sf::Sprite sprite; //Player sprite
    sf::RenderWindow *window; //Pointer to SFML window object
    Coord pos; //Position of the player on screen
    int state; //Primary state of the player (range 0-13) (see Figure 5)
    int s; //Secondary state variable (see Figure 5)
    
public:
    enum WalkDirection {Left,Up,Right,Down};
    /*
    @brief
        Initializes player object
    @params
        window: SFML window object
        textBasePath: This is the path to the folder that contains player textures
        numTextures: Number of player textures (should be 14)
        pos: Player position
    */
    void init(sf::RenderWindow *window, std::string textBasePath, int numTextures, Coord pos);

    /*
    @brief
        Sets player position
    @params
        pos: position
    */
    void setPosition(Coord pos);

    /*
    @brief
        Checks whether player collides with one of the other objects
    @params
        speed: Player movement speed, used when checking boundaries
        dir: One of the WalkDirection enum values (Left, Up, Right, Down)
        barrels: Pointer to barrel objects
        sandbags: Pointer to sandbag objects
        nb: Number of barrel objects
        ns: Number of sandbag objects
    */
    bool checkCollision(float speed, WalkDirection dir, Barrel *barrels, Sandbag *sandbags, int nb, int ns);

    /*
    @brief
        Moves the player around
    @params
        speed: Player movement speed
        dir: One of the WalkDirection enum values (Left, Up, Right, Down)
        barrels: Pointer to barrel objects
        nb: Number of barrel objects
        ns: Number of sandbag objects
    */
    void walk(float speed, WalkDirection dir, Barrel *barrels, Sandbag *sandbags, int nb, int ns);

    //Draws the player sprite
    void paint();
};

class Game
{
    float speed; //Game speed 
    int numBarrels; //Number of barrel objects
    int numSandbags; //Number of sandbag objects
    int numPlayers; //Number of player objects
    int width; //Game screen width
    int height; //Game screen height
    sf::RenderWindow* window; //SFML window object
    sf::Texture bgTexture; //Background tile (grass) texture
    sf::Sprite bgSprite; //Background tile (grass) sprite
    Barrel *barrels; //Pointer to barrel objects
    Sandbag *sandbags; //Pointer to sandbag objects
    Player* players; //Pointer to player objects
public:
    /*
    @brief
        Non-default constructor
    @params
        s: game speed
        w: game window width
        h: game window height
        nb: number of barrel objects
        ns: number of sandbag objects
        np: number of player objects 
    */
    Game(float s, int w, int h, int nb, int ns, int np);

    ~Game();

    //Draws game background, which includes the grasses, sandbags and barrels.
    //initWarzone() must be called before calling this function!
    void drawBackground();
    //initializes war zone by determining locations for objects. this function does not draw objects!
    void initWarzone();
    //main game loop
    void update();
};

Coord::Coord()
{
    this->x=0;
    this->y=0;
}

Coord::Coord(float x, float y)
{
    this->x=x;
    this->y=y;
}

void Sandbag::init(sf::RenderWindow *window, std::string texturePath, Coord pos)
{
    this->window = window;
    this->pos = pos;
    texture.loadFromFile(texturePath);
    sprite.setTexture(texture);
    sprite.setPosition(pos.x,pos.y);
}

Coord Sandbag::getPosition()
{
    return pos;
}

void Sandbag::paint()
{
    window->draw(sprite);
}

void Barrel::init(sf::RenderWindow *window, std::string texturePath, Coord pos)
{
    this->window = window;
    this->pos = pos;
    texture.loadFromFile(texturePath);
    sprite.setTexture(texture);
    sprite.setPosition(pos.x,pos.y);
}

Coord Barrel::getPosition()
{
    return pos;
}

void Barrel::paint()
{
    window->draw(sprite);
}

void Player::init(sf::RenderWindow *window, std::string textBasePath, int numTextures, Coord pos)
{
    this->window = window;
    this->pos = pos;
    state = 0;
    s = 0;
    for (int i = 0; i < 14; i++)
    {
        std::string tmp = textBasePath + "/soldier" + std::to_string(i) + ".png";
        textures[i].loadFromFile(tmp);
    }
    sprite.setTexture(textures[0]);
    sprite.setPosition(pos.x,pos.y);
}

bool Player::checkCollision(float speed, WalkDirection dir, Barrel *barrels, Sandbag *sandbags, int nb, int ns)
{
    if(dir == Up)
    {
        //check collision with barrels
        for (int i = 0; i < nb; i++)
        {
            //check if we are colliding with the barrel. we first check the x-axis, then the y-axis.
            if(pos.x > barrels[i].getPosition().x - 55 && pos.x < (barrels[i].getPosition().x + 20 ) )
            {
                if(barrels[i].getPosition().y + 25 > pos.y && barrels[i].getPosition().y < pos.y)
                    return true;
            }
        }

        //check collision with sandbags
        for (int i = 0; i < ns; i++)
        {
            //check if we are colliding with the sandbag. we first check the x-axis, then the y-axis.
            if(pos.x > sandbags[i].getPosition().x - 55 && pos.x < (sandbags[i].getPosition().x + 30 ) )
            {
                if(sandbags[i].getPosition().y + 35 > pos.y && sandbags[i].getPosition().y < pos.y)
                    return true;
            }
        }

        //check if the soldier is out of bounds. if speed is greater than 20, this may not work. 
        return pos.y + 16 - speed < 0;
    }
    //Likewise for the other directions... You can play with the numbers to tweak the hitbox of the objects.
    else if(dir == Right)
    {
        for (int i = 0; i < nb; i++)
        {
            if(pos.y > barrels[i].getPosition().y - 70 && pos.y < (barrels[i].getPosition().y + 15 ) )
            {
                if(barrels[i].getPosition().x - 70 < pos.x && barrels[i].getPosition().x > pos.x)
                    return true;
            }
        }

        for (int i = 0; i < ns; i++)
        {
            if(pos.y > sandbags[i].getPosition().y - 70 && pos.y < (sandbags[i].getPosition().y + 20 ) )
            {
                if(sandbags[i].getPosition().x - 80 < pos.x && sandbags[i].getPosition().x > pos.x)
                    return true;
            }
        }
        //check if the soldier is out of bounds. if speed is greater than 20, this may not work. 
        return pos.x + 90 + speed > window->getSize().x;
    }
    else if(dir == Left)
    {
        for (int i = 0; i < nb; i++)
        {
            if(pos.y > barrels[i].getPosition().y - 70 && pos.y < (barrels[i].getPosition().y + 15 ) )
            {
                if(barrels[i].getPosition().x + 40 > pos.x && barrels[i].getPosition().x < pos.x)
                    return true;
            }
        }

        for (int i = 0; i < ns; i++)
        {
            if(pos.y > sandbags[i].getPosition().y - 70 && pos.y < (sandbags[i].getPosition().y + 20 ) )
            {
                if(sandbags[i].getPosition().x + 40 > pos.x && sandbags[i].getPosition().x < pos.x)
                    return true;
            }
        }
        //check if the soldier is out of bounds. if speed is greater than 20, this may not work. 
        return pos.x - speed < 0;
    }
    else //dir == Down
    {
        for (int i = 0; i < nb; i++)
        {
            if(pos.x > barrels[i].getPosition().x - 55 && pos.x < (barrels[i].getPosition().x + 20 ) )
            {
                if(barrels[i].getPosition().y - 80 < pos.y && barrels[i].getPosition().y > pos.y)
                    return true;
            }
        }

        for (int i = 0; i < ns; i++)
        {
            if(pos.x > sandbags[i].getPosition().x - 55 && pos.x < (sandbags[i].getPosition().x + 30 ) )
            {
                if(sandbags[i].getPosition().y - 80 < pos.y && sandbags[i].getPosition().y > pos.y)
                    return true;
            }
        }
        //check if the soldier is out of bounds. if speed is greater than 20, this may not work. 
        return pos.y + 95 + speed > window->getSize().y;
    }
}

void Player::walk(float speed, WalkDirection dir, Barrel *barrels, Sandbag *sandbags, int nb, int ns)
{
    //State machine for the soldier, exactly as shown in the document.
    switch (state)
    {
    case 0:
        if(dir == Up && s==1)
        {
            state = 8;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;
            sprite.move(0,-speed);
            pos.y -= speed;
        }
        else if(dir == Up && s==0)
        {
            state = 7;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;              
            sprite.move(0,-speed);
            pos.y -= speed;
        }
        else if(dir == Right || dir == Down) //turn clockwise
        {
            state = 1;
            sprite.setTexture(textures[state]);
        }
        else if(dir == Left) //turn counter-clockwise
        {
            state = 7;
            sprite.setTexture(textures[state]);
        }
        break;

    case 1:
        if(dir == Up || dir == Left) //turn counter-clockwise
        {
            state = 0;
            sprite.setTexture(textures[state]);            
        }
        else if(dir == Down || dir == Right) //turn clockwise
        {
            state = 2;
            sprite.setTexture(textures[state]);
        }
        break;

    case 2:
        if(dir == Right && s==1)
        {
            state = 9;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;
            sprite.move(speed,0);
            pos.x += speed;
        }
        else if(dir == Right && s==0)
        {
            state = 10;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;            
            sprite.move(speed,0);
            pos.x += speed;
        }
        else if(dir == Left || dir == Down) //turn clockwise
        {
            state = 3;
            sprite.setTexture(textures[state]);
        }
        else if(dir == Up) //turn counter-clockwise
        {
            state = 1;
            sprite.setTexture(textures[state]);
        }
        break;

    case 3:
        s = 1;
        if(dir == Down) //Walk down 
        {
            state = 4;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;            
            sprite.move(0,speed);
            pos.y += speed;            
        }
        else if(dir == Left || dir == Up) //Turn clockwise
        {
            state = 4;
            sprite.setTexture(textures[state]);
        }
        else if(dir == Right) //turn counter-clockwise
        {
            state = 2;
            sprite.setTexture(textures[state]);            
        }
        break;

    case 4:
        if(dir == Down && s == 0)
        {
            state = 3;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;            
            sprite.move(0,speed);
            pos.y += speed;
        }
        else if(dir == Down && s == 1)
        {
            state = 11;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;            
            sprite.move(0,speed);
            pos.y += speed;
        }
        else if(dir == Left || dir == Up) //turn clockwise
        {
            state = 5;
            sprite.setTexture(textures[state]);
        }
        else if(dir == Right) //turn counter-clockwise
        {
            state = 3;
            sprite.setTexture(textures[state]);
        }
        break;

    case 5:
        if(dir == Left || dir == Up) //turn clockwise
        {
            state = 6;
            sprite.setTexture(textures[state]);     
        }
        else if(dir == Right || dir == Down) //turn counter-clockwise
        {
            state = 4;
            sprite.setTexture(textures[state]);
        }
        break;

    case 6:
        if(dir == Left && s == 0)
        {
            state = 13;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;
            sprite.move(-speed,0);
            pos.x -= speed;
        }
        else if(dir == Left && s == 1)
        {
            state = 12;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;            
            sprite.move(-speed,0);
            pos.x -= speed;
        }
        else if(dir == Up || dir == Right) //turn clockwise
        {
            state = 7;
            sprite.setTexture(textures[state]);
        }
        else if(dir == Down) //turn counter-clockwise
        {
            state = 5;
            sprite.setTexture(textures[state]);
        }
        break;

    case 7:
        s = 1;
        if(dir == Up) //walk up
        {
            state = 0;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;           
            sprite.move(0,-speed);
            pos.y -= speed;
        }
        else if(dir == Right) //turn clockwise
        {
            state = 0;
            sprite.setTexture(textures[state]);            
        }
        else if(dir == Left || dir == Down) //turn counter-clockwise
        {
            state = 6;
            sprite.setTexture(textures[state]);              
        }
        break;

    case 8:
        state = 0;
        s = 0;
        sprite.setTexture(textures[state]);
        if(dir == Up) //walk up
        {
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;           
            sprite.move(0,-speed);
            pos.y -= speed;            
        }
        break;

    case 9:
        state = 2;
        s = 0;
        sprite.setTexture(textures[state]);
        if(dir == Right) //walk right
        {
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;
            sprite.move(speed,0);
            pos.x += speed;            
        }
        break;

    case 10:
        state = 2;
        s = 1;
        sprite.setTexture(textures[state]);
        if(dir == Right) //walk right
        {
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;
            sprite.move(speed,0);
            pos.x += speed;            
        }        
        break;

    case 11:
        state = 4;
        s = 0;
        sprite.setTexture(textures[state]);
        if(dir == Down) //walk down
        {
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;            
            sprite.move(0,speed);
            pos.y += speed;            
        }
        break;

    case 12:
        state = 6;
        s = 0;
        sprite.setTexture(textures[state]);
        if(dir == Left)
        {
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;
            sprite.move(-speed,0);
            pos.x -= speed;            
        }
        break;

    case 13:
        state = 6;
        s = 1;
        sprite.setTexture(textures[state]);
        if(dir == Left)
        {
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;
            sprite.move(-speed,0);
            pos.x -= speed;            
        }        
        break;                                                
    default:
        break;
    }
}

void Player::paint()
{
    window->draw(sprite);
}

Game::Game(float s, int w, int h, int nb, int ns, int np)
{
    speed = s;
    width = w;
    height = h;
    numBarrels = nb;
    numSandbags = ns;
    numPlayers = np;

    window = new sf::RenderWindow(sf::VideoMode(width, height), "Battlefield 3");
    window->setFramerateLimit(10); //Set frame limit to prevent soldier from sliding while walking
    bgTexture.loadFromFile("textures/grass.png");
    bgSprite.setTexture(bgTexture);

    barrels = new Barrel[nb];
    sandbags = new Sandbag[ns];
    players = new Player[np];
}

Game::~Game()
{
    delete window;
    delete[] sandbags;
    delete[] barrels;
    delete[] players;
}

void Game::initWarzone()
{
    //create a grid for possible object locations. each object has a size of 60x92, approximately.
    int object_grid_width = width/60;
    int object_grid_height = height/92;
    int object_grid_size = object_grid_height * object_grid_width;
    int *object_grid = new int[object_grid_size]; //2d array, 0 means the cell is empty, 1 means the cell is full.
    for (int i = 0; i < object_grid_size; i++)
    {
        object_grid[i] = 0; //all cells in the grid are initally empty...
    }
    
    //mt19937 engine for generating random cell coordinates. rand() would also work,
    //but this is C++ way of doing it.
    std::random_device rd{};
    std::mt19937 gen{rd()};

    std::uniform_int_distribution<int> random_width(0, object_grid_width-1);
    std::uniform_int_distribution<int> random_height(0, object_grid_height-1);

    //randomly spawn sandbags across the field.
    for (int i = 0; i < numSandbags; i++)
    {
        while (1)
        {
            //generate random coordinates
            int coord_x = random_width(gen);
            int coord_y = random_height(gen);
            //convert coordinates to an array index
            int array_index = coord_y == 0 ? object_grid_width*coord_y + coord_x : object_grid_width*(coord_y-1) + coord_x;
            //check if the generated coordinate is full
            if(object_grid[array_index] != 1)
            {
                sandbags[i].init(window,"textures/bags.png",Coord(60*coord_x,92*coord_y));
                object_grid[array_index] = 1;
                break;
            }            
        }
    }
    //randomly spawn barrels across the field. identical to sandbag algorithm.
    for (int i = 0; i < numBarrels; i++)
    {
        while(1)
        {
            int coord_x = random_width(gen);
            int coord_y = random_height(gen);
            int array_index = coord_y == 0 ? object_grid_width*coord_y + coord_x : object_grid_width*(coord_y-1) + coord_x;
            if(object_grid[array_index] != 1)
            {
                barrels[i].init(window,"textures/barrel.png",Coord(60*coord_x,92*coord_y));
                object_grid[array_index] = 1;
                break;
            }
        }
    }

    //randomly spawn the soldier accross the field.
    while(1)
    {
        int coord_x = random_width(gen);
        int coord_y = random_height(gen);
        int array_index = coord_y == 0 ? object_grid_width*coord_y + coord_x : object_grid_width*(coord_y-1) + coord_x;
        if(object_grid[array_index] != 1)
        {
            players[0].init(window,"textures",14,Coord(60*coord_x,92*coord_y));
            players[0].paint();
            break;
        }
    }
    window->display();
    delete[] object_grid; //don't forget to free the memory.
}

void Game::drawBackground()
{
    //draw grasses
    for (int i = 0; i < width; i+=350)
    {
        for (int j = 0; j < height; j+=350)
        {
            bgSprite.setPosition(i,j);
            window->draw(bgSprite);
        }
    }
    //draw barrels
    for (int i = 0; i < numBarrels; i++)
    {
        barrels[i].paint();
    }
    //draw sandbags
    for (int i = 0; i < numSandbags; i++)
    {
        sandbags[i].paint();
    }
}

void Game::update()
{
    //main game loop
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
            else if(event.type == sf::Event::KeyPressed)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                    players[0].walk(speed,players[0].Up,barrels,sandbags,numBarrels,numSandbags);
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                    players[0].walk(speed,players[0].Down,barrels,sandbags,numBarrels,numSandbags);
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                    players[0].walk(speed,players[0].Right,barrels,sandbags,numBarrels,numSandbags);
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                    players[0].walk(speed,players[0].Left,barrels,sandbags,numBarrels,numSandbags);
            }
        }
        window->clear();

        this->drawBackground();
        players[0].paint();

        window->display();
    }
}

int main()
{
    //You can choose arbitrary window size, and arbitrary numbers of sandbags and barrels.
    //The program should draw the background with no trouble.
    //However, if you choose very large numbers for objects, the program might not start because it might
    //not be able to find an empty cell for every object.
    //You can play with the speed, but I found "4" to be working well.
    Game mygame(4,1024,768,20,20,1);
    mygame.initWarzone(); //determine locations for objects
    mygame.update(); //main game loop
    return 0;   
}

//compile commmand for linux
//g++ main.cpp -lsfml-graphics -lsfml-window -lsfml-system

