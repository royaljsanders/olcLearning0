#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using namespace std;

/* compile with:
g++ -o olcLearning0 olcLearning0.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17
*/
class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

// remember protected? I sure don't
private:
    wstring sLevel; //level represented as a string
    int nLevelWidth;
    int nLevelHeight;

    // Player Properties
    float fPlayerPosX = 1.0f;
    float fPlayerPosY = 1.0f;
    float fPlayerVelX = 0.0f;
    float fPlayerVelY = 0.0f;

    //camera position
    float fCameraPosX = 0.0f;
    float fCameraPosY = 0.0f;

// remember protected? I sure don't
protected:
    // why virtual?
	virtual bool OnUserCreate() override
	{
        //size of the level in tiles
		nLevelWidth = 64;
        nLevelHeight = 16;

        sLevel += L"................................................................";
        sLevel += L"................................................................";
        sLevel += L"................................................................";
        sLevel += L"................................................................";
        sLevel += L"......................#.........................................";
        sLevel += L"................................................................";
        sLevel += L"..#####.............####........................................";
        sLevel += L"....#...........................................................";
        sLevel += L"................................................................";
        sLevel += L"########################.##########....#########################";
        sLevel += L".......................#.#............#.........................";
        sLevel += L".......................#.#...........#..........................";
        sLevel += L".......................#.#..........#...........................";
        sLevel += L".......................#...........#............................";
        sLevel += L".......................############.............................";
        sLevel += L"................................................................";

        return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) override
	{
        // Utility Lambdas
        auto GetTile = [&](int x, int y)
        {
            // make sure it's in bounds for X and Y
            if (x >= 0 && x < nLevelWidth && y >= 0 && y < nLevelHeight)
                return sLevel[y * nLevelWidth + x];
            else
                return L' '; //gives us a space if an error occurs
        };

        auto SetTile = [&](int x, int y, wchar_t c)
        {
            if (x >= 0 && x < nLevelWidth && y >= 0 && y < nLevelHeight)
                sLevel[y*nLevelWidth + x] = c;
        };

        // Handle Input
        fPlayerVelX = 0.0f;
        fPlayerVelY = 0.0f;

        // not sure if supported
        if (IsFocused()) {
            if (GetKey(olc::Key::UP).bHeld)     fPlayerVelY = -6.0f;
            if (GetKey(olc::Key::DOWN).bHeld)   fPlayerVelY =  6.0f;
            if (GetKey(olc::Key::LEFT).bHeld)   fPlayerVelX = -6.0f;
            if (GetKey(olc::Key::RIGHT).bHeld)  fPlayerVelX =  6.0f;

        }

        // Apply new position
        fPlayerPosX = fPlayerPosX + fPlayerVelX * fElapsedTime;
        fPlayerPosY = fPlayerPosY + fPlayerVelY * fElapsedTime;

        // Link camera to player position
        fCameraPosX = fPlayerPosX;
        fCameraPosY = fPlayerPosY;

        // Draw Level
        // each tile is 16x16 pixels
        int nTileWidth = 16;
        int nTileHeight = 16;

        int nVisibleTilesX = ScreenWidth() / nTileWidth;
        int nVisibleTilesY = ScreenWidth() / nTileHeight;

        // we want camera to be in middle
        // offset from top left (0,0) to middle of ScreenHeight
        // result is top-leftmost visible tile relative to camera?
        float fOffsetX = fCameraPosX - (float)nVisibleTilesX / 2.0f;
        float fOffsetY = fCameraPosY - (float)nVisibleTilesY / 2.0f;

        // "clamp" camera to map boundaries: if new offset goes out of range, then prevent it
        if (fOffsetX < 0) fOffsetX = 0;
        if (fOffsetY < 0) fOffsetY = 0;
        if (fOffsetX > nLevelWidth - nVisibleTilesX) fOffsetX = nLevelWidth - nVisibleTilesX;
        if (fOffsetY > nLevelHeight - nVisibleTilesY) fOffsetY = nLevelHeight - nVisibleTilesY;

        // Get offsets for smooth movement
        float fTileOffsetX = (fOffsetX - (int)fOffsetX) * nTileWidth;
        float fTileOffsetY = (fOffsetY - (int)fOffsetY) * nTileHeight;

        // Draw visible tile map
        for (int x = 0; x < nVisibleTilesX + 1; x++)
        {
            for (int y = 0; y < nVisibleTilesY + 1; y++)
            {
                // offset moving us further into the 2d array
                wchar_t sTileId = GetTile(x + fOffsetX, y + fOffsetY);
                switch (sTileId)
                {
                    case L'.':
                        // empty, draw background rectangle.
                        FillRect(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, nTileWidth, nTileHeight, olc::CYAN);
                        break;
                    case L'#':
                        // block
                        FillRect(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, nTileWidth, nTileHeight, olc::RED);
                        break;
                    default:
                        // draw black
                        FillRect(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, nTileWidth, nTileHeight, olc::BLACK);
                }
            }
        }

        // Draw Player
        // subtract offset from player position to represent end of level case
        // offset moving the player back into the screen
        FillRect((fPlayerPosX - fOffsetX) * nTileWidth,
                 (fPlayerPosY - fOffsetY) * nTileWidth,
                 nTileWidth,
                 nTileHeight,
                 olc::GREEN);




		return true;
	}
};


int main()
{
	Example demo;
    if (demo.Construct(160, 80,4,4))
		demo.Start();

	return 0;
}
