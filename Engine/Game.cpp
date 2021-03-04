/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	playerPos(Graphics::ScreenWidth/2.0f, Graphics::ScreenHeight/2.0f),
	playerVel(0.0f, 0.0f)
{
	//Creating the borders
	for (size_t j = 0; j < blockLength; j++)
	{
		for (size_t i = 0; i < blockLength; i++)
		{
			const int id = blockLength * j + i;
			if (i == 0 || i == blockLength - 1 || j == 0 || j == blockLength - 1)
			{
				blocks[id].color = Color(255, 204, 153);
			}
			blocks[id].leftTop = Vec2(float(i * blockSide), float(j * blockSide));
			blocks[id].bottomRight = Vec2(float((i+1) * blockSide), float((j+1) * blockSide));
		}
	}
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	//Getting the delta time
	const float DT = ft.Mark();

	//World update
	if (wnd.mouse.LeftIsPressed())
	{
		const size_t id = GetBlockId(wnd.mouse.GetPos() / blockSide);

		if (blocks[id].color == Colors::Black)
		{
			blocks[id].color = Color(255, 204, 153);
		}
	}
	if (wnd.mouse.RightIsPressed())
	{
		const size_t id = GetBlockId(wnd.mouse.GetPos() / blockSide);

		if (blocks[id].color == Color(255, 204, 153))
		{
			blocks[id].color = Colors::Black;
		}
	}

	//Player update
	Controls(DT);
	Physics(DT);
}

void Game::DrawRect(Vec2 a, Vec2 b, const Color& col, bool outline)
{
	if (a.x > b.x)
	{
		std::swap(a.x, b.x);
	}
	if (a.y > b.y)
	{
		std::swap(a.y, b.y);
	}

	if (outline)
	{
		gfx.DrawLine(Vec2(a.x, a.y), Vec2(b.x - 1.0f, a.y - 1.0f), col);
		gfx.DrawLine(Vec2(a.x, b.y), Vec2(b.x - 1.0f, b.y - 1.0f), col);
		gfx.DrawLine(Vec2(a.x, a.y), Vec2(a.x-1.0f, b.y-1.0f), col);
		gfx.DrawLine(Vec2(b.x, a.y), Vec2(b.x-1.0f, b.y-1.0f), col);
	}
	else
	{
		for (int j = int(a.y); j < int(b.y); j++)
		{
			for (int i = int(a.x); i < int(b.x); i++)
			{
				if (i >= 0 && i < Graphics::ScreenWidth && j >= 0 && j < Graphics::ScreenHeight)
				{
					gfx.PutPixel(i, j, col);
				}
			}
		}
	}
}

Vei2 Game::GetPlayerBlockPos() const
{
	return GetBlockPos(GetPlayerBlockId());
}

size_t Game::GetPlayerBlockId() const
{
	return GetBlockId(Vei2(int(playerPos.x/blockSide), int(playerPos.y / blockSide)));
}

void Game::ComposeFrame()
{
	//Drawing the world
	for (size_t j = 0; j < blockLength; j++)
	{
		for (size_t i = 0; i < blockLength; i++)
		{
			const size_t id = j * blockLength + i;
			DrawRect(blocks[id].leftTop, blocks[id].bottomRight, blocks[id].color, false);
		}
	}

	//Drawing the player
	DrawRect(playerPos, playerPos + Vec2(playerWidth, playerHeight), Colors::Magenta, false);

	//Debugging
	Vec2 bpLeftTop(playerPos);
	Vec2 bpBottomRight(playerPos);

	if (playerVel.x > 0.0f)
	{
		bpBottomRight.x += playerWidth + playerVel.x;
	}
	if (playerVel.x < 0.0f)
	{
		bpLeftTop.x += playerVel.x;
		bpBottomRight.x += playerWidth;
	}
	if (playerVel.x == 0.0f)
	{
		bpBottomRight.x += playerWidth;
	}

	if (playerVel.y > 0.0f)
	{
		bpBottomRight.y += playerHeight + playerVel.y;
	}
	if (playerVel.y < 0.0f)
	{
		bpLeftTop.y += playerVel.y;
		bpBottomRight.y += playerHeight;
	}
	if (playerVel.y == 0.0f)
	{
		bpBottomRight.y += playerHeight;
	}
	DrawRect(bpLeftTop, bpBottomRight, Colors::Blue, true);
}
