
#ifndef _GRID_WORLD_H
#define _GRID_WORLD_H

#include <vector>
#include <iostream>

using std::vector;

struct Node
{
  unsigned ID;
  Node *prev = nullptr;
  Node *next = nullptr;
};
struct Person
{
  unsigned ID;
  unsigned row;
  unsigned col;
  bool alive = true;
  Node *node;
  Person(unsigned newRow, unsigned newCol, unsigned newID, Node *newNode) : row(newRow), col(newCol), ID(newID), node(newNode) {}
};
struct District
{
  unsigned row;
  unsigned col;
  unsigned population = 0;
  Node *head = nullptr;
  // Node* tail; maybe useful?
};
class GridWorld
{

private:
  unsigned maxRow = 0;
  unsigned maxCol = 0;
  unsigned nextUnused = 0;
  unsigned totalPop = 0;
  Node *usedNum = nullptr;
  Node *usedNumBack = nullptr;
  vector<Person *> people;
  vector<vector<District *>> districts;
  vector<int>* rtn=new vector<int>();

  void freeHead(Node *head)
  {
    Node *cur = head;
    while (cur != nullptr)
    {
      Node *temp = cur->next;
      free(cur);
      cur = temp;
    }
  }

  // private stuff goes here!
  //   typedefs
  //   data members
  //   private helper functions
  //   etc.

public:
  /**
   * constructor:  initializes a "world" with nrows and
   *    ncols (nrows*ncols districtcs) in which all
   *    districtricts are empty (a wasteland!).
   */
  GridWorld(unsigned nrows, unsigned ncols)
  {
    maxRow = nrows;
    maxCol = ncols;
    districts = vector<vector<District *>>(maxRow, vector<District *>(maxCol));
    // districts(maxRow,vector<District*>(maxCol));
    for (int i = 0; i < maxRow; i++)
    {
      for (int j = 0; j < maxCol; j++)
      {
        District *temp = new District();
        temp->row = i;
        temp->col = j;
        districts.at(i).at(j) = temp;
      }
    }

    // your constructor code here!
  }
  ~GridWorld()
  {
    for (int i = 0; i < maxRow; i++)
    {
      for (int j = 0; j < maxCol; j++)
      {
        freeHead(districts.at(i).at(j)->head);
        free(districts.at(i).at(j));
      }
    }
    for (int i = 0; i < people.size(); i++)
    {
      free(people.at(i));
    }
    // your destructor code here.
  }

  /*
   * function: birth
   * description:  if row/col is valid, a new person is created
   *   with an ID according to rules in handout.  New person is
   *   placed in district (row, col)
   *
   * return:  indicates success/failure
   */
  bool birth(int row, int col, int &id)
  {
    if (row >= maxRow || col >= maxCol)
    {
      return false;
    }
    unsigned tempID;

    if (usedNum != nullptr)
    {
      tempID = usedNum->ID;
      Node *temp = usedNum;
      usedNum = usedNum->next;
      free(temp);
    }
    else
    {
      tempID = nextUnused;
      nextUnused++;
    }
    Node *newNode = new Node();

    newNode->ID = tempID;
    newNode->next = districts.at(row).at(col)->head;
    districts.at(row).at(col)->population++;
    totalPop++;

    if (districts.at(row).at(col)->head == nullptr)
    {
      districts.at(row).at(col)->head = newNode;
    }
    else
    {
      districts.at(row).at(col)->head->prev = newNode;
      districts.at(row).at(col)->head = newNode;
    }
    Person *tempPerson = new Person(row, col, tempID, newNode);
    if (tempID >= people.size())
    {
      people.push_back(tempPerson);
    }
    else
    {
      Person* temp=people.at(tempID);
      people.at(tempID) = tempPerson;
      free(temp);
    }

    id = tempPerson->ID;
    return true;
  }

  /*
   * function: death
   * description:  if given person is alive, person is killed and
   *   data structures updated to reflect this change.
   *
   * return:  indicates success/failure
   */
  bool death(int personID)
  {
    if (personID >= nextUnused || !people.at(personID)->alive)
    {
      return false;
    }

    Person *dead = people.at(personID);
    Node *temp = dead->node;

    if (temp->prev != nullptr)
    {
      temp->prev->next = temp->next;
    }
    else if (districts.at(dead->row).at(dead->col)->head == temp)
    {
      districts.at(dead->row).at(dead->col)->head = temp->next;
    }
    else
    {
      std::cout << "Something went wrong in death\n";
    }

    if (temp->next != nullptr)
    {
      temp->next->prev = temp->prev;
    }
    free(dead->node);
    dead->alive = false;

    unsigned row = dead->row;
    unsigned col = dead->col;

    districts.at(row).at(col)->population--;
    totalPop--;

    Node *cur = usedNum;
    Node *newUsed = new Node();

    newUsed->ID = dead->ID;
    if (cur == nullptr)
    {
      cur = newUsed;
    }
    if (usedNumBack == nullptr)
    {
      usedNumBack = newUsed;
    }
    else
    {
      usedNumBack->next = newUsed;
      usedNumBack = newUsed;
    }
    return true;
  }

  /*
   * function: whereis
   * description:  if given person is alive, his/her current residence
   *   is reported via reference parameters row and col.
   *
   * return:  indicates success/failure
   */
  bool
  whereis(int id, int &row, int &col) const
  {
    if (id >= nextUnused || !people.at(id)->alive)
    {
      return false;
    }
    row = people.at(id)->row;
    col = people.at(id)->col;
    return true;
  }

  /*
   * function: move
   * description:  if given person is alive, and specified target-row
   *   and column are valid, person is moved to specified district and
   *   data structures updated accordingly.
   *
   * return:  indicates success/failure
   *
   * comment/note:  the specified person becomes the 'newest' member
   *   of target district (least seniority) --  see requirements of members().
   */


  //unlink node- update prev and next if not null
  //relink node- make it front of new district, change its next, prev and update the header of new if its previouly empty to be it, or change the prev on a node already there to it
  //change the row and col represented in the people vector
  // change new and old populations
  bool move(int id, int targetRow, int targetCol)
  {
    if (id >= nextUnused || !people.at(id)->alive || targetRow >= maxRow || targetCol >= maxCol)
    {
      return false;
    }

    Person *movedPerson = people.at(id);

    unsigned oldRow = movedPerson->row;
    unsigned oldCol = movedPerson->col;

    districts.at(oldRow).at(oldCol)->population--;
    districts.at(targetRow).at(targetCol)->population++;

    movedPerson->row = targetRow;
    movedPerson->col = targetCol;

    Node *tempNode = movedPerson->node;

    if (tempNode->prev != nullptr)
    {
      tempNode->prev->next = tempNode->next;
    }
    else if (districts.at(oldRow).at(oldCol)->head == tempNode)
    {
      districts.at(oldRow).at(oldCol)->head = tempNode->next;
    }
    else
    {
      std::cout << "Something went wrong in move\n";
    }

    if (tempNode->next != nullptr)
    {
      tempNode->next->prev = tempNode->prev;
    }

    if (districts.at(targetRow).at(targetCol)->head == nullptr)
    {
      districts.at(targetRow).at(targetCol)->head = tempNode;
      tempNode->next=nullptr;
    }
    else
    {
      districts.at(targetRow).at(targetCol)->head->prev = tempNode;
      tempNode->next=districts.at(targetRow).at(targetCol)->head;
      districts.at(targetRow).at(targetCol)->head = tempNode;
    }

    tempNode->prev=nullptr;

    return true;
  }

  /** func: members(r,c)
   *  returns a snapshot of the members of specified
   *    district as a vector (ordered by seniority).
   *  If invalid district, an empty vector is
   *    returned
   */
  std::vector<int> *members(int row, int col) const
  {
    rtn->clear();
    vector<int>* temp=new vector<int>();
    if (row >= maxRow || col >= maxCol || districts.at(row).at(col)->population == 0)
    {
      return temp;
    }
    Node *cur = districts.at(row).at(col)->head;
    while (cur->next != nullptr)
    {
      cur = cur->next;
    }
    while (cur != nullptr)
    {
      temp->push_back(cur->ID);
      cur = cur->prev;
    }
    return temp;
  }

  /*
   * function: population
   * description:  returns the current (living) population of the world.
   */
  int population() const
  {
    return totalPop;
  }

  /*
   * function: population(int,int)
   * description:  returns the current (living) population of specified
   *   district.  If district does not exist, zero is returned
   */
  int population(int row, int col) const
  {
    if (row >= maxRow || col >= maxCol)
    {
      return 0;
    }
    return districts.at(row).at(col)->population;
  }

  /*
   * function: num_rows
   * description:  returns number of rows in world
   */
  int num_rows() const
  {
    return maxRow;
  }

  /*
   * function: num_cols
   * description:  returns number of columns in world
   */
  int num_cols() const
  {
    return maxCol;
  }
};

#endif
