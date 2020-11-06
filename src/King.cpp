#include "King.h"

#include <cmath>
#include <cstring>
#include <iostream>

King::King()
{
    this->SetEmpty();
}

King::~King()
{
    //dtor
}

King::King(int x, int y, bool col)
{
    this->x_ = x;
    this->y_ = y;

    if(col) {
        this->name = "WK";
        this->color = Color::WHITE;
    } else {
        this->name = "BK";
        this->color = Color::BLACK;
    }
}

bool King::ValidateMove(int x1, int y1,
                        int x2, int y2, const Field& f) const
{
    bool empty = f.get(x2, y2).IsEmpty();
    bool diff_color = f.get(x2, y2).GetColor() != this->GetColor();

    Vector2D<int> diff;
    diff.x = std::abs(x2 - x1);
    diff.y = std::abs(y2 - y1);

    if(((empty || diff_color) && (diff.x <= 1 && diff.y <= 1)) && f.IsPathClear({x1, y1}, {x2, y2})) {
            return true;
    }
    return false;
}

bool King::Move(int x, int y, Field& f)
{
    if(x == this->x_ && y == this->y_) {
        return false;
    }

    // Check for castling
    bool col = this->IsWhite();
    const char* rook = col ? "WR" : "BR";

    int rookCoordX = -1;
    if(x == 6) 
    {
        rookCoordX = 7;
    }
    if(x == 1) {
        rookCoordX = 0;
    }

    Vector2D<int> oldCoords{this->x_, this->y_};
    Vector2D<int> dist{x - this->x_, y - this->y_};

    if(
        rookCoordX != -1 
        && std::strcmp(f.get(rookCoordX, y).GetName(), rook) == 0 
        && !f.get(rookCoordX, y).IsEmpty() &&  this->firstMove 
        && f.get(x, y).GetFirstMove() && !this->wasThreatened 
        && f.IsPathClear(oldCoords, {x, y})
    ) {
        // You thought this is it?
        // No, now you've got to check that the king won't get under attack
        // on the cells that he's passing through.

        int step = dist.x > 0 ? 1 : -1;

        f.swap(this->x_, this->y_, this->x_ + step, this->y_);
        if(!f.KingInDanger(*this)) {
            f.swap(this->x_, this->y_, this->x_ + step, this->y_);
            if(!f.KingInDanger(*this)) {
                std::cout << "Castling successful\n";
                if(step < 0) f.swap(0, y, 3, y);
                else f.swap(7, y, 5, y);
                return true;
            }
        }

        f.swap(this->x_, this->y_, oldCoords.x, oldCoords.y);
        std::cout << "Castling failed. King would have been threatened.\n";
    }

    if(ValidateMove(this->x_, this->y_, x, y, f))
    {
        f.get(x, y).SetEmpty();
        f.swap(this->x_, this->y_, x, y);

        this->firstMove = false;

        return true;
    }

    return false;
}

