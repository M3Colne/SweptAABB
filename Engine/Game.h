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
	Game(class MainWindow& wnd);
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();
	/********************************/
	/*  User Functions              */
	void DrawRect(Vec2 a, Vec2 b, const Color& col, bool outline);
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
			playerVel.x -= playerXspeed * DT;
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			playerVel.x += playerXspeed * DT;
		}

		//Jumping
		const size_t blockUnderPlayerId = GetPlayerBlockId() + int(blockLength * playerHeight / blockSide);
		if (blockUnderPlayerId < blockSize)
			if (!(blocks[blockUnderPlayerId].color == Colors::Black))
			{
				grounded = true;
			}
			else
			{
				grounded = false;
			}
		if (wnd.kbd.KeyIsPressed('W') && grounded)
		{
			playerVel.y = -playerJumpImpulse;
			grounded = false;
		}
	}
	float SweptAABB(const Block& block1, const Block& block2, const Vec2& vel, Vec2& normal) const
	{
		//Get the distances
		Vec2 entryDist(block2.leftTop.x - block1.bottomRight.x, block2.leftTop.y - block1.bottomRight.y);
		Vec2 exitDist(block2.bottomRight.x - block1.leftTop.x, block2.bottomRight.y - block1.leftTop.y);
		if (vel.x < 0.0f)
		{
			std::swap<float>(entryDist.x, exitDist.x);
		}
		if (vel.y < 0.0f)
		{
			std::swap<float>(entryDist.y, exitDist.y);
		}

		//Get the times
		Vec2 entryTimeV(entryDist.x / vel.x, entryDist.y / vel.y);
		Vec2 exitTimeV(exitDist.x / vel.x, exitDist.y / vel.y);
		if (vel.x == 0.0f)
		{
			entryTimeV.x = -std::numeric_limits<float>::infinity();
			exitTimeV.x = std::numeric_limits<float>::infinity();
		}
		if (vel.y == 0.0f)
		{
			entryTimeV.y = -std::numeric_limits<float>::infinity();
			exitTimeV.y = std::numeric_limits<float>::infinity();
		}

		//Getting the entry and exit time points
		const float entryTime = std::max<float>(entryTimeV.x, entryTimeV.y);
		const float exitTime = std::min<float>(exitTimeV.x, exitTimeV.y);

		//Collision test
		if (entryTime > exitTime || (entryTimeV.x < 0.0f && entryTimeV.y < 0.0f) || entryTimeV.x > 1.0f || entryTimeV.y > 1.0f)
		{
			normal.x = 0.0f;
			normal.y = 0.0f;
			return 1.0f;
		}
		else
		{
			if (entryTimeV.x > entryTimeV.y)
			{
				if (entryDist.x < 0.0f)
				{
					normal.x = 1.0f;
					normal.y = 0.0f;
				}
				else
				{
					normal.x = -1.0f;
					normal.y = 0.0f;
				}
			}
			else
			{
				if (entryDist.y < 0.0f)
				{
					normal.x = 0.0f;
					normal.y = 1.0f;
				}
				else
				{
					normal.x = 0.0f;
					normal.y = -1.0f;
				}
			}
			return entryTime; 
		}
	}
	void Physics(const float DT)
	{
		//Forces
		if (!grounded)
		{
			playerVel.y += gravityAcc * DT;//External forces(Ex: Gravity)
		}

		//Collision detection
		Vec2 minNormal(0.0f, 0.0f);
		float minCollisionTime = 1.0f;
		const Vec2 playerVelFrame = playerVel * DT; //This is the velocity that we are working with, not the playerVel!
		if (playerVel.x || playerVel.y) //This is testing if the player is even moving at all, if not then we don't do any physics
		{
			//Broadphashing
			Vec2 bpLeftTop(playerPos);
			Vec2 bpBottomRight(playerPos);

			if (playerVelFrame.x > 0.0f)
			{
				bpBottomRight.x += playerWidth + playerVelFrame.x;
			}
			if (playerVelFrame.x < 0.0f)
			{
				bpLeftTop.x += playerVelFrame.x;
				bpBottomRight.x += playerWidth;
			}
			if (playerVelFrame.x == 0.0f)
			{
				bpBottomRight.x += playerWidth;
			}

			if (playerVelFrame.y > 0.0f)
			{
				bpBottomRight.y += playerHeight + playerVelFrame.y;
			}
			if (playerVelFrame.y < 0.0f)
			{
				bpLeftTop.y += playerVelFrame.y;
				bpBottomRight.y += playerHeight;
			}
			if (playerVelFrame.y == 0.0f)
			{
				bpBottomRight.y += playerHeight;
			}

			//Get each block in the world that is in the broadphase in this vector
			std::vector<Block*> collidableBlocks;
			for (size_t j = 0; j < blockLength; j++)
			{
				for (size_t i = 0; i < blockLength; i++)
				{
					const size_t id = j * blockLength + i;
					if (!(blocks[id].color == Colors::Black))
					{
						if (bpBottomRight.x > blocks[id].leftTop.x &&
							bpLeftTop.x < blocks[id].bottomRight.x &&
							bpBottomRight.y > blocks[id].leftTop.y &&
							bpLeftTop.y < blocks[id].bottomRight.y)
						{
							collidableBlocks.push_back(&blocks[id]);
						}
					}
				}
			}

			for (auto block : collidableBlocks)
			{
				Vec2 normal(0.0f, 0.0f);
				const float collisionTime = SweptAABB(Block(playerPos, playerPos + Vec2(playerWidth, playerHeight),
					Colors::Black), *block, playerVelFrame, normal);

				if (collisionTime < minCollisionTime)
				{
					minCollisionTime = collisionTime;
					minNormal = normal;
				}
			}
		}

		//Integration(yes, integrate after collision detection and then after this we might integrate again i collision response if
		//a collision happend)
		playerPos += playerVelFrame * minCollisionTime;

		//Collision response(sliding)
		if (minCollisionTime < 1.0f)
		{
			const float remainingTime = 1.0f - minCollisionTime;
			const float notDotP = playerVel.x * minNormal.y + playerVel.y * minNormal.x;
			playerVel.x = notDotP * minNormal.y;
			playerVel.y = notDotP * minNormal.x;
			playerPos += playerVel * remainingTime * DT;
		}
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
	static constexpr unsigned int blockSize = blockLength * blockLength;
	Block blocks[blockSize];
	static constexpr float gravityAcc = blockSide*2.0f;

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