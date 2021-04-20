#include <iostream>
#include <string>
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

    /*
    @brief
    Returns sandbag position
    */
    Coord getPosition();

    /*
    @brief
    Draws the sandbag sprite
    */
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

    /*
    @brief
    Returns barrel position
    */
    Coord getPosition();

    /*
    @brief
    Draws the barrel sprite
    */
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
    barrels: Pointer to barrel objects
    sandbags: Pointer to sandbag objects
    nb: Number of barrel objects
    ns: Number of sandbag objects
    */
    bool checkCollision(Barrel *barrels, Sandbag *sandbags, int nb, int ns);

    /*
    @brief
    Moves the player around
    @params
    speed: Player movement speed (you can pick any speed unit you wish)
    dir: One of the WalkDirection enum values (Left, Up, Right, Down)
    barrels: Pointer to barrel objects
    nb: Number of barrel objects
    ns: Number of sandbag objects
    */
    void walk(float speed, WalkDirection dir, Barrel *barrels, Sandbag *sandbags, int nb, int ns);

    /*
    @brief
    Draws the player sprite
    */
    void paint();
};

class Game
{
    float speed; //Game speed (you can pick any speed unit you wish)
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

    //Draws game background
    void drawBackground();
    //Draws all objects and updates screen    
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

void Player::walk(float speed, WalkDirection dir, Barrel *barrels, Sandbag *sandbags, int nb, int ns)
{
    switch (state)
    {
    case 0:
        if(dir == Up && s==1)
        {
            state = 8;
            sprite.setTexture(textures[state]);
            sprite.move(0,-speed);
        }
        else if(dir == Up && s==0)
        {
            state = 7;
            sprite.setTexture(textures[state]);
            sprite.move(0,-speed);
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
            sprite.move(speed,0);
        }
        else if(dir == Right && s==0)
        {
            state = 10;
            sprite.setTexture(textures[state]);
            sprite.move(speed,0);
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
        if(dir == Down || dir == Left || dir == Up) //Walk down or turn clockwise
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
            sprite.move(0,speed);
        }
        else if(dir == Down && s == 1)
        {
            state = 11;
            sprite.setTexture(textures[state]);
            sprite.move(0,speed);
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
            sprite.move(-speed,0);
        }
        else if(dir == Left && s == 1)
        {
            state = 12;
            sprite.setTexture(textures[state]);
            sprite.move(-speed,0);
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
        if(dir == Up)
        {
            state = 0;
            sprite.setTexture(textures[state]);
            sprite.move(0,-speed);            
        }
        else if(dir == Right)
        {
            state = 0;
            sprite.setTexture(textures[state]);            
        }
        else if(dir == Left || dir == Down)
        {
            state = 6;
            sprite.setTexture(textures[state]);              
        }
        break;

    case 8:
        state = 0;
        s = 0;
        sprite.setTexture(textures[state]);
        sprite.move(0,-speed);
        break;

    case 9:
        state = 2;
        s = 0;
        sprite.setTexture(textures[state]);
        sprite.move(speed,0);
        break;

    case 10:
        state = 2;
        s = 1;
        sprite.setTexture(textures[state]);
        sprite.move(speed,0);    
        break;

    case 11:
        state = 4;
        s = 0;
        sprite.setTexture(textures[state]);
        sprite.move(0,speed);    
        break;

    case 12:
        state = 6;
        s = 0;
        sprite.setTexture(textures[state]);
        sprite.move(-speed,0);    
        break;

    case 13:
        state = 6;
        s = 1;
        sprite.setTexture(textures[state]);
        sprite.move(-speed,0);       
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

    window = new sf::RenderWindow(sf::VideoMode(700, 700), "Battlefield 3");
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

void Game::drawBackground()
{
    for (int i = 0; i < width; i+=350)
    {
        for (int j = 0; i < height; j+=350)
        {
            bgSprite.setPosition(i,j);
            window->draw(bgSprite);
        }
    }
    window->display();
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(700, 700), "Battlefield 3");
    sf::Texture grass_texture;
    grass_texture.loadFromFile("textures/grass.png");
    sf::Sprite grass_sprite[4];
    grass_sprite[0].setTexture(grass_texture);
    grass_sprite[1].setTexture(grass_texture);
    grass_sprite[2].setTexture(grass_texture);
    grass_sprite[3].setTexture(grass_texture);
    grass_sprite[1].setPosition(350,0);
    grass_sprite[2].setPosition(350,350);
    grass_sprite[3].setPosition(0,350);

    std::string sandbag_path = "textures/bags.png";
    std::string barrel_path = "textures/barrel.png";
    Sandbag s1,s2,s3,s4;
    s1.init(&window,sandbag_path,Coord(13,13));
    s2.init(&window,sandbag_path,Coord(600,75));
    s3.init(&window,sandbag_path,Coord(223,432));
    s4.init(&window,sandbag_path,Coord(500,198));
    Barrel b1,b2,b3,b4;
    b1.init(&window,barrel_path,Coord(79,67));
    b2.init(&window,barrel_path,Coord(350,75));
    b3.init(&window,barrel_path,Coord(444,432));
    b4.init(&window,barrel_path,Coord(139,550));
    Player p1;
    p1.init(&window,"textures",14,Coord(350,350));
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if(event.type == sf::Event::KeyPressed)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                    p1.walk(3,p1.Up,nullptr,nullptr,0,0);
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                    p1.walk(3,p1.Down,nullptr,nullptr,0,0);
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                    p1.walk(3,p1.Right,nullptr,nullptr,0,0);
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                    p1.walk(3,p1.Left,nullptr,nullptr,0,0);
            }
                
        }

        window.clear();
        window.draw(grass_sprite[0]);
        window.draw(grass_sprite[1]);
        window.draw(grass_sprite[2]);
        window.draw(grass_sprite[3]);
        s1.paint();
        s2.paint();
        s3.paint();
        s4.paint();
        b1.paint();
        b2.paint();
        b3.paint();
        b4.paint();
        p1.paint();
        window.display();
        sf::sleep(sf::milliseconds(100));
    }
    return 0;   
}
