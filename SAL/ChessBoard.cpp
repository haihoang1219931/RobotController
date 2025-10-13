#include "ChessBoard.h"
#include <string.h>

ChessBoard::ChessBoard(float x, float y, float size):
    m_chessBoardPosX(x),
    m_chessBoardPosY(y),
    m_chessBoardRect(size)
{
    memset(m_dropZoneMap,0,16);
    memset(m_dropZoneMapGuest,0,16);
}

float ChessBoard::getChessBoardPosX()
{
    return m_chessBoardPosX;
}

float ChessBoard::getChessBoardPosY()
{
    return m_chessBoardPosY;
}

float ChessBoard::getChessBoardSize()
{
    return m_chessBoardRect*8;
}

void ChessBoard::setChessBoardPosX(float value)
{
    m_chessBoardPosX = value;
}

void ChessBoard::setChessBoardPosY(float value)
{
    m_chessBoardPosY = value;
}

void ChessBoard::setChessBoardSize(float value)
{
    m_chessBoardRect = value/8;
}

Point ChessBoard::getFreeDropPoint(uint8_t promotePiece)
{
    Point freePoint;
    if(promotePiece == 0) {
        for(int rowId = 0; rowId < 8; rowId ++){
            for(int colId = 0; colId < 8; colId ++){
                if(m_dropZoneMap[rowId][colId] == 0) {
                    freePoint =
                    break;
                }
            }
        }
    } else {
        for(int rowId = 0; rowId < 8; rowId ++){
            for(int colId = 0; colId < 8; colId ++){
                if(m_dropZoneMapGuest[rowId][colId] == promotePiece) {

                    break;
                }
            }
        }
    }
    return freePoint;
}

Point ChessBoard::convertPoint(int row, int col)
{
    int centerCol = 0;
    int centerRow = 0;
    Point convertValue;
    float squareLength = m_chessBoardRect;
    convertValue.x = (float)(col-centerCol) * squareLength
            + squareLength/2 + m_chessBoardPosX;
    convertValue.y = (float)(row-centerRow) * squareLength
            + squareLength/2 + m_chessBoardPosY;
    return convertValue;
}

Point ChessBoard::convertDropPoint(int row, int col, bool guestZone) {
    Point convertValue;
    convertValue.x = guestZone?
                m_chessBoardPosX+8*m_chessBoardRect+m_dropZoneSpace+
                    col*m_chessBoardRect + m_chessBoardRect/2:
                -(m_chessBoardPosX+4*m_chessBoardRect+m_dropZoneSpace+
                    (1-col)*m_chessBoardRect + m_chessBoardRect/2);
    convertValue.y = m_chessBoardPosY +
                    row * m_chessBoardRect +
                    m_chessBoardRect/2;
    return convertValue;
}

void ChessBoard::updateDropZone(uint8_t piece, int row, int col, bool guestZone)
{
    if(guestZone) {
        m_dropZoneMapGuest[row][col] = piece;
    } else {
        m_dropZoneMap[row][col] = piece;
    }
}

void ChessBoard::moveGuestPieceOut(uint8_t piece) {
    for(int rowId = 0; rowId < 8; rowId ++){
        for(int colId = 0; colId < 8; colId ++){
            if(m_dropZoneMap[rowId][colId] == 0) {
                m_dropZoneMap[rowId][colId] = piece;
                break;
            }
        }
    }
}

void ChessBoard::promotePiece(uint8_t piece) {

}
