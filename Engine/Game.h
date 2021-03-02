/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
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
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Vec2.h"
#include "FrameTimer.h"

struct Block
{
	Vec2 leftTop = { 0.0f, 0.0f };
	Vec2 bottomRight = { 0.0f, 0.0f };
	Color color = Colors::Black;

	Block() = default;
	Block(Vec2 a, Vec2 b, Color c)
		:
		leftTop(a),
		bottomRight(b),
		color(c)
	{}
};

class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();
	/********************************/
	/*  User Functions              */
	void DrawRect(Vec2 a, Vec2 b, const Color& col);
	Vei2 GetBlockPos(size_t id) const
	{
		const auto y = id % blockLength;
		return Vei2(id - y * blockLength, y);
	}
	size_t GetBlockId(const Vei2 pos) const
	{
		return pos.y * blockLength + pos.x;
	}
	Vei2 GetPlayerBlockPos() const;
	size_t GetPlayerBlockId() const;

	void Controls(const float DT)
	{
		//Side moving
		if (wnd.kbd.KeyIsPressed('A'))
		{
			playerPos.x -= playerXspeed * DT;
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			playerPos.x += playerXspeed * DT;
		}

		//Jumping
		if (!(blocks[GetPlayerBlockId() + int(blockLength * playerHeight / blockSide)].color == Colors::Black))
		{
			grounded = true;
		}
		if (wnd.kbd.KeyIsPressed('W') && grounded)
		{
			playerVel.y = -playerJumpImpulse;
			grounded = false;
		}
	}
	void Physics(const float DT)
	{
		//Forces
		if (!grounded)
		{
			playerVel.y += gravityAcc * DT;//External forces(Ex: Gravity)
		}

		//Integration
		playerPos += playerVel * DT;

		//Collisions
		//---
	}
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
	//World
	FrameTimer ft;
	static constexpr unsigned int blockSide = 10;
	static constexpr unsigned int blockLength = Graphics::ScreenWidth / blockSide;
	Block blocks[blockLength * blockLength];
	static constexpr float gravityAcc = blockSide*10.0f;

	//Player
	Vec2 playerPos;
	Vec2 playerVel;
	static constexpr float playerWidth = float(blockSide);
	static constexpr float playerHeight = float(blockSide*2.0f);
	static constexpr float playerXspeed = blockSide * 10.0f;
	static constexpr float playerJumpImpulse = blockSide * 6.0f;
	bool grounded = false;
	/********************************/
};