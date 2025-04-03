// Projectile.h
#ifndef PROJECTILE_H
#define PROJECTILE_H

struct Projectile {
    int proj_row;
    int proj_col;
    bool active;

    Projectile(int row, int col) : proj_row(row), proj_col(col), active(true) {}

    void move()  {proj_col++;}
};

#endif // PROJECTILE_H
