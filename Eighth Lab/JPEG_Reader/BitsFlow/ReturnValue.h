//
// Created by Nik Carlson on 09.06.2021.
//

#ifndef EIHTH_LAB_RETURNVALUE_H
#define EIHTH_LAB_RETURNVALUE_H

enum BitStatus {
    F,
    OK,
    END
};

struct BitResult {
    BitStatus status;
    bool res;

    BitResult() : status(F), res(false) {};

    BitResult(BitStatus status, bool res) : status(status), res(res) {
    }
};

#endif //EIHTH_LAB_RETURNVALUE_H
