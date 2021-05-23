#include <iostream>
#include <string>
#include <sstream>
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

//Abstract base class. Player, Barrel, Sandbag and Bullet are derived from this class.
class Object
{
protected:
    sf::RenderWindow* window; //SFML window object
    sf::Texture texture; //Object texture
    sf::Sprite sprite; //Object sprite
    Coord pos; //Object position
public:

    /*
    @brief
        Initializes an object
    @params
        window: SFML window object
        texturePath: This is the path to the object texture file
        pos: Object position
    */
    virtual void init(sf::RenderWindow *window, std::string texturePath, Coord pos);

    //Returns object position
    Coord getPosition();

    //Draws the object sprite
    void paint();

    //Returns object sprite
    const sf::Sprite getSprite();

    //Make this class abstract
    virtual ~Object() =0;
};

class Sandbag: public Object
{

};

class Barrel: public Object
{
    //Visibility of the barrel. 1 is visible, 0 is invisible.
    int isVisible;
public:
    //Inherited function
    void init(sf::RenderWindow *window, std::string texturePath, Coord pos);

    //Returns isVisible
    const bool getVisible();
    //Sets isVisible
    void setVisible(bool visible);
};

class Bullet: public Object
{
    //Declare BulletList as friend, so it can access the "next" pointer.
    friend class BulletList;

    float speed; //Speed of the bullet
    Bullet *next; //Next bullet in the linked list
public:
    enum TravelDirection {Left,Up,Right,Down};
private:
    TravelDirection dir; //Travel direction of the bullet
public:
    //Inherited function
    void init(sf::RenderWindow *window, std::string texturePath, Coord pos);

    //Moves the sprite in the travel direction, and updates the position accordingly.
    void move();

    //Sets bullet speed
    void setSpeed(float speed);

    //Sets the bullet travel direction
    void setDirection(TravelDirection dir);

    //Returns the bullet travel direction
    const TravelDirection getDirection();
};

class Player: public Object
{
    sf::Texture textures[14]; //Player texture array (one element per soldier state)
    int state; //Primary state of the player (range 0-13)
    int s; //Secondary state variable
    int score; //Score of the player
    Coord initPos; //Positing at the beginning of the game. This is used when the game is restarted.

public:
    enum WalkDirection {Left,Up,Right,Down,None};
private:
    //Input buffer. This array holds the 2 most recent pressed keys. This ensures a smoother movement,
    //especially when changing directions.
    WalkDirection pressedDir[2];
public:

    //Inherited functions
    void init(sf::RenderWindow *window, std::string texturePath, Coord pos);

    //Sets player position
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

    //Returns the current travel direction of the player, which is the first element in the pressedDir array.
    WalkDirection getPressed();

    //Appends a new direction to the pressedDir array
    void setPressed(WalkDirection dir);

    //Clears the direction from the pressedDir array
    void clearPressed(WalkDirection dir);

    //Returns the state variable
    const int getState();

    //Returns true if the soldier is in an appropriate state to shoot. A soldier can only shoot a bullet
    //if the rifle is pointing up, down, left or right; but not diagonal.
    const bool canShoot();

    //Returns the current score of the player
    const int getScore();

    //Increments score by 1
    void incrementScore();
};

class BulletList
{
    sf::RenderWindow* window; //SFML window object
    Bullet *list; //Head of the linked list
public:
    BulletList(sf::RenderWindow* window);

    //Adds a new bullet to the list at the given coordinate and speed.
    //The state parameter is needed to determine if the bullet needs a 90 degree rotation.
    void add(Coord pos, int state, float speed);

    //Moves every bullet in the list.
    void update();

    //Paints every bullet in the list.
    void paint();

    //Iterates through the linked list and check collision for every bullet. A bullet is destroyed when
    //it collides with a sandbag, barrel or a soldier.
    void checkCollision(Player* players, Barrel* barrels, Sandbag* sandbags, int np, int nb, int ns);

    //Deletes the linked list.
    ~BulletList();
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

    sf::Text text; //Text object
    sf::Font font; //Font object

    BulletList *bullets; //Linked list for bullets
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

    //Initializes war zone by determining locations for objects. this function does not draw objects!
    void initWarzone();

    //Draws game background, which includes the grasses, sandbags and barrels.
    //initWarzone() must be called before calling this function!
    void drawBackground();

    //Main game loop. Returns 1 when the game is restarted, returns 0 when the game needs to exit.
    int update();
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

void Object::init(sf::RenderWindow *window, std::string texturePath, Coord pos)
{
    this->window = window;
    this->pos = pos;
    texture.loadFromFile(texturePath);
    sprite.setTexture(texture);
    sprite.setPosition(pos.x,pos.y);
}

Coord Object::getPosition()
{
    return pos;
}

const sf::Sprite Object::getSprite()
{
    return sprite;
}

void Object::paint()
{
    window->draw(sprite);
}

Object::~Object() {}

void Barrel::init(sf::RenderWindow *window, std::string texturePath, Coord pos)
{
    this->window = window;
    this->pos = pos;
    texture.loadFromFile(texturePath);
    sprite.setTexture(texture);
    sprite.setPosition(pos.x,pos.y);
    isVisible = 1;
}

const bool Barrel::getVisible()
{
    return isVisible;
}

void Barrel::setVisible(bool visible)
{
    isVisible = visible;
}

void Bullet::init(sf::RenderWindow *window, std::string texturePath, Coord pos)
{
    this->window = window;
    this->pos = pos;
    texture.loadFromFile(texturePath);
    sprite.setTexture(texture);
    sprite.setPosition(pos.x,pos.y);

    //Direction and speed are arbitrary here. 
    //Use setDirection and setSpeed to change them.
    dir = Left;
    speed = 0;
    next = nullptr;
}

void Bullet::move()
{
    if(dir == Up)
    {
        sprite.move(0,-speed);
        pos.y -= speed;
    }
    else if(dir == Down)
    {
        sprite.move(0,speed);
        pos.y += speed;
    }
    else if(dir == Left)
    {
        sprite.move(-speed,0);
        pos.x -= speed;
    }
    else if(dir == Right)
    {
        sprite.move(speed,0);
        pos.x += speed;
    }
}

void Bullet::setSpeed(float speed)
{
    this->speed = speed;
}

void Bullet::setDirection(TravelDirection dir)
{
    this->dir = dir;
    //Rotate the bullet sprite if necessary.
    if(dir == TravelDirection::Left || dir == TravelDirection::Right)
        sprite.rotate(90.f);
}

const Bullet::TravelDirection Bullet::getDirection()
{
    return dir;
}

BulletList::BulletList(sf::RenderWindow* window)
{
    this->window = window;
    list = nullptr;
}

void BulletList::add(Coord pos, int state, float speed)
{
    //Determine the bullet direction and position based on soldier's state.
    //The position is determined so that the bullet comes out from the tip of the rifle.
    Bullet::TravelDirection bullet_dir;
    Coord bullet_pos;

    if(state == 0 || state == 7 || state == 8)
    {
        bullet_dir = Bullet::Up;
        bullet_pos.x = pos.x + 60;
        bullet_pos.y = pos.y - 2 ;
    }
    else if(state == 2 || state == 9 || state == 10)
    {
        bullet_dir = Bullet::Right;
        bullet_pos.x = pos.x + 109;
        bullet_pos.y = pos.y + 75;
    }
    else if(state == 6 || state == 12 || state == 13)
    {
        bullet_dir = Bullet::Left;
        bullet_pos.x = pos.x + 5;
        bullet_pos.y = pos.y + 38;
    }
    else if(state == 3 || state == 4 || state == 11)
    {
        bullet_dir = Bullet::Down;
        bullet_pos.x = pos.x + 30;
        bullet_pos.y = pos.y + 95;
    }

    //Now insert the bullet into the linked list.
    //See if we are inserting to the head of the list.
    if(list == nullptr)
    {
        list = new Bullet;
        list->init(window,"textures/bullet.png",bullet_pos);
        list->setDirection(bullet_dir);
        list->setSpeed(speed);
    }
    else
    {
        Bullet* tmp_ptr = list;
        //Find the tail of the list
        while(tmp_ptr->next != nullptr)
        {
            tmp_ptr = tmp_ptr->next;
        }
        //Insert to the tail.
        tmp_ptr->next = new Bullet;
        tmp_ptr = tmp_ptr->next;
        tmp_ptr->init(window,"textures/bullet.png",bullet_pos);
        tmp_ptr->setDirection(bullet_dir);
        tmp_ptr->setSpeed(speed);
    }
}

void BulletList::checkCollision(Player* players, Barrel* barrels, Sandbag* sandbags, int np, int nb, int ns)
{
    Bullet *current;
    Bullet *previous = nullptr;
    //We use the sf::Rect::intersects() function to check for collision.
    sf::Rect<float> bullet_rect; //Rectangle object for bullet
    sf::Rect<float> object_rect; //Rectangle object for the object

    //Check collision with players first.
    for (int i = 0; i < np; i++)
    {
        object_rect = players[i].getSprite().getGlobalBounds();
        //Adjust player hitbox based on the state.
        int player_state = players[i].getState();
        if(player_state == 0)
        {
            object_rect.height = 38;
            object_rect.width = 40;
            object_rect.top += 37;
            object_rect.left += 25;
        }
        else if(player_state == 1)
        {
            object_rect.height = 38;
            object_rect.width = 40;
            object_rect.top += 37;
            object_rect.left += 25;
        }
        else if(player_state == 2)
        {
            object_rect.height = 42;
            object_rect.width = 37;
            object_rect.top += 37;
            object_rect.left += 33;
        }
        else if(player_state == 3)
        {
            object_rect.height = 36;
            object_rect.width = 45;
            object_rect.top += 38;
            object_rect.left += 24;
        }
        else if(player_state == 4)
        {
            object_rect.height = 35;
            object_rect.width = 42;
            object_rect.top += 42;
            object_rect.left += 26;
        }
        else if(player_state == 5)
        {
            object_rect.height = 35;
            object_rect.width = 34;
            object_rect.top += 42;
            object_rect.left += 30;
        }
        else if(player_state == 6)
        {
            object_rect.height = 36;
            object_rect.width = 36;
            object_rect.top += 38;
            object_rect.left += 23;
        }
        else if(player_state == 7)
        {
            object_rect.height = 37;
            object_rect.width = 38;
            object_rect.top += 38;
            object_rect.left += 26;
        }
        else if(player_state == 8)
        {
            object_rect.height = 37;
            object_rect.width = 34;
            object_rect.top += 41;
            object_rect.left += 27;
        }
        else if(player_state == 9)
        {
            object_rect.height = 35;
            object_rect.width = 34;
            object_rect.top += 43;
            object_rect.left += 29;
        }
        else if(player_state == 10)
        {
            object_rect.height = 35;
            object_rect.width = 33;
            object_rect.top += 43;
            object_rect.left += 32;
        }
        else if(player_state == 11)
        {
            object_rect.height = 33;
            object_rect.width = 33;
            object_rect.top += 42;
            object_rect.left += 31;
        }
        else if(player_state == 12)
        {
            object_rect.height = 34;
            object_rect.width = 37;
            object_rect.top += 39;
            object_rect.left += 26;
        }
        else if(player_state == 13)
        {
            object_rect.height = 34;
            object_rect.width = 37;
            object_rect.top += 39;
            object_rect.left += 26;
        }

        //Iterate through the bullet list and check for collision with each bullet
        current = list;
        while(current != nullptr)
        {
            bullet_rect = current->sprite.getGlobalBounds();
            if(bullet_rect.intersects(object_rect)) //delete bullet if there is collision
            {
                if(current == list) //delete head
                {
                    list = list->next;
                    delete current;
                    current = list;
                }
                else
                {
                    Bullet *tmp = current;
                    current = current->next;
                    delete tmp;
                    previous->next = current;
                }
                //Increment score
                if(i == 0)
                    players[1].incrementScore();
                else
                    players[0].incrementScore();
            }
            else //next bullet
            {
                previous = current;
                current = current->next;
            }
        }
    }
    //Check collision with sandbags
    for (int i = 0; i < ns; i++)
    {
        object_rect = sandbags[i].getSprite().getGlobalBounds();
        object_rect.height = 70;
        current = list;
        while(current != nullptr)
        {
            bullet_rect = current->sprite.getGlobalBounds();
            if(bullet_rect.intersects(object_rect)) //delete bullet
            {
                if(current == list) //delete head
                {
                    list = list->next;
                    delete current;
                    current = list;
                }
                else
                {
                    Bullet *tmp = current;
                    current = current->next;
                    delete tmp;
                    previous->next = current;
                }
            }
            else
            {
                previous = current;
                current = current->next;
            }
        }
    }

    //Check collision with barrels.
    for (int i = 0; i < nb; i++)
    {
        object_rect = barrels[i].getSprite().getGlobalBounds();
        object_rect.height = 70;
        current = list;
        while(current != nullptr && barrels[i].getVisible())
        {
            bullet_rect = current->sprite.getGlobalBounds();
            if(bullet_rect.intersects(object_rect)) //delete bullet
            {
                if(current == list) //delete head
                {
                    list = list->next;
                    delete current;
                    current = list;
                }
                else
                {
                    Bullet *tmp = current;
                    current = current->next;
                    delete tmp;
                    previous->next = current;
                }
                barrels[i].setVisible(false);
            }
            else
            {
                previous = current;
                current = current->next;
            }
        }
    }
}

void BulletList::update()
{
    Bullet *tmp = list;
    while(tmp != nullptr)
    {
        tmp->move();
        tmp = tmp->next;
    }
}

void BulletList::paint()
{
    Bullet *current = list;
    while(current != nullptr)
    {
        current->paint();
        current = current->next;
    }
}

BulletList::~BulletList()
{
    Bullet *current = list;
    Bullet *next = nullptr;
    while(current != nullptr)
    {
        next = current->next;
        delete current;
        current = next;
    }
}

void Player::init(sf::RenderWindow *window, std::string texturePath, Coord pos)
{
    this->window = window;
    this->pos = pos;
    initPos = pos;
    state = 0;
    s = 0;
    score = 0;
    pressedDir[0] = None;
    pressedDir[1] = None;
    for (int i = 0; i < 14; i++)
    {
        std::string tmp = texturePath + "/soldier" + std::to_string(i) + ".png";
        textures[i].loadFromFile(tmp);
    }
    sprite.setTexture(textures[0]);
    sprite.setPosition(pos.x,pos.y);
}

void Player::setPosition(Coord pos)
{
    this->pos = pos;
}

void Player::incrementScore()
{
    score++;
}

const int Player::getScore()
{
    return score;
}

bool Player::checkCollision(float speed, WalkDirection dir, Barrel *barrels, Sandbag *sandbags, int nb, int ns)
{
    if(dir == Up)
    {
        //check collision with barrels
        for (int i = 0; i < nb; i++)
        {
            //check if we are colliding with the barrel. we first check the x-axis, then the y-axis.
            if(pos.x > barrels[i].getPosition().x - 55 && pos.x < (barrels[i].getPosition().x + 20 ) && barrels[i].getVisible())
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

        //check if the soldier is out of bounds.
        return pos.y + 16 - speed < 0;
    }
    //Likewise for the other directions... You can play with the numbers to tweak the hitbox of the objects.
    else if(dir == Right)
    {
        for (int i = 0; i < nb; i++)
        {
            if(pos.y > barrels[i].getPosition().y - 70 && pos.y < (barrels[i].getPosition().y + 15 ) && barrels[i].getVisible())
            {
                if(barrels[i].getPosition().x - 70 < pos.x && barrels[i].getPosition().x > pos.x)
                    return true;
            }
        }

        for (int i = 0; i < ns; i++)
        {
            if(pos.y > sandbags[i].getPosition().y - 70 && pos.y < (sandbags[i].getPosition().y + 20 ))
            {
                if(sandbags[i].getPosition().x - 80 < pos.x && sandbags[i].getPosition().x > pos.x)
                    return true;
            }
        }
        //check if the soldier is out of bounds.
        return pos.x + 90 + speed > window->getSize().x;
    }
    else if(dir == Left)
    {
        for (int i = 0; i < nb; i++)
        {
            if(pos.y > barrels[i].getPosition().y - 70 && pos.y < (barrels[i].getPosition().y + 15 ) && barrels[i].getVisible())
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
        //check if the soldier is out of bounds.
        return pos.x - speed < 0;
    }
    else //dir == Down
    {
        for (int i = 0; i < nb; i++)
        {
            if(pos.x > barrels[i].getPosition().x - 55 && pos.x < (barrels[i].getPosition().x + 20 ) && barrels[i].getVisible())
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
        //check if the soldier is out of bounds.
        return pos.y + 95 + speed > window->getSize().y;
    }
}

void Player::walk(float speed, WalkDirection dir, Barrel *barrels, Sandbag *sandbags, int nb, int ns)
{
    //State machine for the soldier, exactly as shown in the document.
    switch (state)
    {
    case 0:
        if(dir == Up && s==1) //walk up
        {
            state = 8;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;
            sprite.move(0,-speed);
            pos.y -= speed;
        }
        else if(dir == Up && s==0) //walk up
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
        if(dir == Right && s==1) //walk right
        {
            state = 9;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;
            sprite.move(speed,0);
            pos.x += speed;
        }
        else if(dir == Right && s==0) //walk right
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
        if(dir == Down && s == 0) //walk down
        {
            state = 3;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;
            sprite.move(0,speed);
            pos.y += speed;
        }
        else if(dir == Down && s == 1) //walk down
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
        if(dir == Left && s == 0) //walk left
        {
            state = 13;
            sprite.setTexture(textures[state]);
            if(this->checkCollision(speed,dir,barrels,sandbags,nb,ns))
                return;
            sprite.move(-speed,0);
            pos.x -= speed;
        }
        else if(dir == Left && s == 1) //walk left
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
        if(dir == Left) //walk left
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
        if(dir == Left) //walk left
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

Player::WalkDirection Player::getPressed()
{
    return pressedDir[0];
}

void Player::setPressed(WalkDirection dir)
{
    if(pressedDir[0] == None)
        pressedDir[0] = dir;
    else if(pressedDir[1] == None && pressedDir[0] != dir)
        pressedDir[1] = dir;
}

void Player::clearPressed(WalkDirection dir)
{
    if(pressedDir[0] == dir)
    {
        pressedDir[0] = pressedDir[1];
        pressedDir[1] = None;
    }
    else if(pressedDir[1] == dir)
    {
        pressedDir[1] = None;
    }
}

const int Player::getState()
{
    return state;
}

const bool Player::canShoot()
{
    return (state != 1 && state != 5 && state != 3 && state != 7 && state != 10 && state != 13);
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

    font.loadFromFile("font.ttf");
    text.setFont(font);
    text.setCharacterSize(30);

    barrels = new Barrel[nb];
    sandbags = new Sandbag[ns];
    players = new Player[np];

    bullets = new BulletList(window);
}

Game::~Game()
{
    delete window;
    delete[] sandbags;
    delete[] barrels;
    delete[] players;
    delete bullets;
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
    for (int i = 0; i < numPlayers; i++)
    {
        while(1)
        {
            int coord_x = random_width(gen);
            int coord_y = random_height(gen);
            int array_index = coord_y == 0 ? object_grid_width*coord_y + coord_x : object_grid_width*(coord_y-1) + coord_x;
            if(object_grid[array_index] != 1)
            {
                players[i].init(window,"textures",Coord(60*coord_x,92*coord_y));
                players[i].paint();
                object_grid[array_index] = 1;
                break;
            }
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
        if(barrels[i].getVisible()) //draw the barrel only if it is visible
            barrels[i].paint();
    }
    //draw sandbags
    for (int i = 0; i < numSandbags; i++)
    {
        sandbags[i].paint();
    }
}

int Game::update()
{
    //Use clocks to add a cooldown to shooting bullets. Otherwise, players can spam bullets.
    sf::Clock clock0, clock1;
    //Main game loop
    while (window->isOpen())
    {
        //Move the soldiers first.
        for (int i = 0; i < numPlayers; i++)
        {
            if(players[i].getPressed() != Player::None)
                players[i].walk(speed,players[i].getPressed(),barrels,sandbags,numBarrels,numSandbags);
        }
        //Check for collision
        bullets->checkCollision(players,barrels,sandbags,numPlayers,numBarrels,numSandbags);
        bullets->update();

        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                return 0; //Return 0 to indicate that the user exited the game.
            }
            else
            {
                //We track the KeyPressed and KeyReleased events to ensure smooth movement.
                //When a KeyPressed event occurs, the key is inserted into the input buffer.
                //When a KeyReleased event occurs, the key is removed from the input buffer.
                //The first element in the input buffer determines the travel direction of the soldier.
                if(event.type == sf::Event::KeyPressed)
                {
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                        players[0].setPressed(Player::Up);
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                        players[0].setPressed(Player::Down);
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                        players[0].setPressed(Player::Right);
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                        players[0].setPressed(Player::Left);
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && players[0].canShoot() && clock0.getElapsedTime().asMilliseconds() > 100)
                    {
                        bullets->add(players[0].getPosition(),players[0].getState(),speed+25);
                        clock0.restart();
                    }

                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                        players[1].setPressed(Player::Up);
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                        players[1].setPressed(Player::Down);
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                        players[1].setPressed(Player::Right);
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                        players[1].setPressed(Player::Left);
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && players[1].canShoot() && clock1.getElapsedTime().asMilliseconds() > 100)
                    {
                        bullets->add(players[1].getPosition(),players[1].getState(),speed+25);
                        clock1.restart();
                    }

                }
                else if(event.type == sf::Event::KeyReleased)
                {
                    if(event.key.code == sf::Keyboard::Up)
                        players[0].clearPressed(Player::Up);
                    else if(event.key.code == sf::Keyboard::Down)
                        players[0].clearPressed(Player::Down);
                    else if(event.key.code == sf::Keyboard::Right)
                        players[0].clearPressed(Player::Right);
                    else if(event.key.code == sf::Keyboard::Left)
                        players[0].clearPressed(Player::Left);

                    else if(event.key.code == sf::Keyboard::W)
                        players[1].clearPressed(Player::Up);
                    else if(event.key.code == sf::Keyboard::S)
                        players[1].clearPressed(Player::Down);
                    else if(event.key.code == sf::Keyboard::D)
                        players[1].clearPressed(Player::Right);
                    else if(event.key.code == sf::Keyboard::A)
                        players[1].clearPressed(Player::Left);
                }
            }
        }
        window->clear();

        this->drawBackground();
        players[0].paint();
        players[1].paint();
        /*
         for (int i = 0; i < numPlayers; i++)
        {
            sf::Rect<float> object_rect;
            object_rect = players[i].getSprite().getGlobalBounds();
            if(players[i].getState() == 0)
            {
                object_rect.height = 38;
                object_rect.width = 40;
                object_rect.top += 37;
                object_rect.left += 25;
            }

            else if(players[i].getState() == 1)
            {
                object_rect.height = 38;
                object_rect.width = 40;
                object_rect.top += 37;
                object_rect.left += 25;
            }

            else if(players[i].getState() == 2)
            {
                object_rect.height = 42;
                object_rect.width = 37;
                object_rect.top += 37;
                object_rect.left += 33;
            }
            else if(players[i].getState() == 3)
            {
                object_rect.height = 36;
                object_rect.width = 45;
                object_rect.top += 38;
                object_rect.left += 24;
            }
            else if(players[i].getState() == 4)
            {
                object_rect.height = 35;
                object_rect.width = 42;
                object_rect.top += 42;
                object_rect.left += 26;
            }
            else if(players[i].getState() == 5)
            {
                object_rect.height = 35;
                object_rect.width = 34;
                object_rect.top += 42;
                object_rect.left += 30;
            }
            else if(players[i].getState() == 6)
            {
                object_rect.height = 36;
                object_rect.width = 36;
                object_rect.top += 38;
                object_rect.left += 23;
            }
            else if(players[i].getState() == 7)
            {
                object_rect.height = 37;
                object_rect.width = 38;
                object_rect.top += 38;
                object_rect.left += 26;
            }
            else if(players[i].getState() == 8)
            {
                object_rect.height = 37;
                object_rect.width = 34;
                object_rect.top += 41;
                object_rect.left += 27;
            }
            else if(players[i].getState() == 9)
            {
                object_rect.height = 35;
                object_rect.width = 34;
                object_rect.top += 43;
                object_rect.left += 29;
            }
            else if(players[i].getState() == 10)
            {
                object_rect.height = 35;
                object_rect.width = 33;
                object_rect.top += 43;
                object_rect.left += 32;
            }
            else if(players[i].getState() == 11)
            {
                object_rect.height = 33;
                object_rect.width = 33;
                object_rect.top += 42;
                object_rect.left += 31;
            }
            else if(players[i].getState() == 12)
            {
                object_rect.height = 34;
                object_rect.width = 37;
                object_rect.top += 39;
                object_rect.left += 26;
            }
            else if(players[i].getState() == 13)
            {
                object_rect.height = 34;
                object_rect.width = 37;
                object_rect.top += 39;
                object_rect.left += 26;
            }

            sf::RectangleShape hitbox;
            hitbox.setSize(sf::Vector2f(object_rect.width,object_rect.height));
            hitbox.setPosition(object_rect.left,object_rect.top);
            hitbox.setFillColor(sf::Color(250, 10, 50,128));
            window->draw(hitbox);
        }*/
        bullets->paint();

        std::stringstream ss;
        if(players[0].getScore() == 10 || players[1].getScore() == 10) //Someone won the game...
        {
            if(players[0].getScore() == 10) //Player 1 wins
                ss << "Player 1 wins\nStart over? (Y/N)";
            else //Player 2 wins
                ss << "Player 2 wins\nStart over? (Y/N)";

            std::string scoreboard = ss.str();
            text.setPosition(width/2 - 140, height/2 - 40); //Write the text at the middle of the scren
            text.setString(scoreboard);
            window->draw(text);
            window->display();
            //Wait for a keyboard input
            sf::Event event;
            while (1)
            {
                while(!window->pollEvent(event));
                if(event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::N))
                    return 0;
                else if(event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
                    return 1;
            }
        }
        else
        {
            ss << "Player 1 score: " << players[0].getScore() << "\nPlayer 2 score: " << players[1].getScore();
            std::string scoreboard = ss.str();
            text.setPosition(width/2 - 140, height-70);
            text.setString(scoreboard);
            window->draw(text);
            window->display();
        }
    }
    return 0;
}

int main()
{
    //You can choose arbitrary window size, and arbitrary numbers of sandbags and barrels.
    //The program should draw the background with no trouble.
    //However, if you choose very large numbers for objects, the program might not start because it might
    //not be able to find an empty cell for every object.
    //You can play with the speed, but I found "10" to be working well.
    Game *gameptr;
    while (1)
    {
        gameptr = new Game(10,1024,768,3,3,2);
        gameptr->initWarzone(); //determine locations for objects

        if(gameptr->update())
            delete gameptr;
        else
        {
            delete gameptr;
            break;
        }
    }
    return 0;
}

//compile commmand for linux
//g++ main.cpp -lsfml-graphics -lsfml-window -lsfml-system
