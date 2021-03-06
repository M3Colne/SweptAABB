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
		if (wnd.kbd.KeyIsPressed('W'))
		{
			playerVel.y -= playerXspeed * DT;
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			playerVel.y += playerXspeed * DT;
		}

		//Jumping
		/*const size_t blockUnderPlayerId = GetPlayerBlockId() + int(blockLength * playerHeight / blockSide);
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
		}*/
	}

	bool AABB_CollisionDetection(const Vec2& leftTop0, const Vec2& bottomRight0, const Vec2& leftTop1, const Vec2& bottomRight1) const;
	float SweptAABB(const Block& block1, const Block& block2, const Vec2& vel, Vei2& normal) const
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
		Vec2 entryTimeV(0.0f, 0.0f);
		Vec2 exitTimeV(0.0f, 0.0f);
		if (vel.x == 0.0f)
		{
			entryTimeV.x = -std::numeric_limits<float>::infinity();
			exitTimeV.x = std::numeric_limits<float>::infinity();
		}
		else
		{
			entryTimeV.x = entryDist.x / vel.x;
			exitTimeV.x = exitDist.x / vel.x;
		}
		if (vel.y == 0.0f)
		{
			entryTimeV.y = -std::numeric_limits<float>::infinity();
			exitTimeV.y = std::numeric_limits<float>::infinity();
		}
		else
		{
			entryTimeV.y = entryDist.y / vel.y;
			exitTimeV.y = exitDist.y / vel.y;
		}

		//Getting the entry and exit time points
		const float entryTime = std::max<float>(entryTimeV.x, entryTimeV.y);
		const float exitTime = std::min<float>(exitTimeV.x, exitTimeV.y);

		//Collision test
		if (entryTime > exitTime || (entryTimeV.x < 0.0f && entryTimeV.y < 0.0f) || entryTimeV.x > 1.0f || entryTimeV.y > 1.0f)
		{
			normal.x = 0;
			normal.y = 0;
			return 1.0f;
		}
		else
		{
			if (entryTimeV.x >= entryTimeV.y)
			{
				if (entryDist.x < 0)
				{
					normal.x = 1;
					normal.y = 0;
				}
				else
				{
					normal.x = -1;
					normal.y = 0;
				}
			}
			else
			{
				if (entryDist.y < 0)
				{
					normal.x = 0;
					normal.y = 1;
				}
				else
				{
					normal.x = 0;
					normal.y = -1;
				}
			}
			return entryTime;
		}
	}
	void CollisionFix(const Vec2& vel, std::vector<Block*>& collidableBlocks, float& collisionTime, Vei2& normal)
	{
		//Find the smallest time for a possible collision and remove that block that we collided with
		size_t collidingBlockId = 0;
		for (size_t i = 0; i < collidableBlocks.size(); i++)
		{
			Vei2 n(0, 0);
			const float ct = SweptAABB(Block(playerPos, playerPos + Vec2(playerWidth, playerHeight),
				Colors::Black), *collidableBlocks[i], vel, n);

			if (ct < collisionTime)
			{
				collisionTime = ct;
				normal = n;
				collidingBlockId = i;
			}
		}
		if (!collidableBlocks.empty())
		{
			collidableBlocks.erase(collidableBlocks.begin() + collidingBlockId);
		}

		//Integration
		//Actually this is more like a collision fix(we move the player so that it just touches the block)
		playerPos += vel * collisionTime;
	}
	void Physics(const float DT)
	{
		//Forces
		//if (!grounded)
		//{
		//	playerVel.y += gravityAcc * DT;//External forces(Ex: Gravity)
		//}

		//Collision detection
		Vei2 minNormal(0, 0);
		float minCollisionTime = 1.0f;
		std::vector<Block*> collidableBlocks;

		//This is testing if the player is even moving at all, if not then we don't do any collisionFixing
		if (playerVel.x || playerVel.y)
		{
			const Vec2 playerVelFrame = playerVel * DT; //This is the velocity that we are working with, not the playerVel!

			//Broadphashing
			Vec2 bpLeftTop(playerPos);
			Vec2 bpBottomRight(playerPos.x + playerWidth, playerPos.y + playerHeight);

			if (playerVelFrame.x > 0.0f)
			{
				bpBottomRight.x += playerVelFrame.x;
			}
			if (playerVelFrame.x < 0.0f)
			{
				bpLeftTop.x += playerVelFrame.x;
			}
			if (playerVelFrame.y > 0.0f)
			{
				bpBottomRight.y += playerVelFrame.y;
			}
			if (playerVelFrame.y < 0.0f)
			{
				bpLeftTop.y += playerVelFrame.y;
			}

			//Get each block in the world that is in the broadphase in this vector
			for (size_t j = 0; j < blockLength; j++)
			{
				for (size_t i = 0; i < blockLength; i++)
				{
					const size_t id = j * blockLength + i;
					if (!(blocks[id].color == Colors::Black))
					{
						if (AABB_CollisionDetection(bpLeftTop, bpBottomRight, blocks[id].leftTop, blocks[id].bottomRight))
						{
							collidableBlocks.push_back(&blocks[id]);
						}
					}
				}
			}

			//Fix the collision(Move the player just at the edge of the block)
			CollisionFix(playerVelFrame, collidableBlocks, minCollisionTime, minNormal);
		}

		//Collision response(sliding)
		if (minCollisionTime < 1.0f)
		{
			const float notDotP = playerVel.x * minNormal.y + playerVel.y * minNormal.x;
			playerVel.x = notDotP * minNormal.y;
			playerVel.y = notDotP * minNormal.x;

			//This part of the algorithm should be fixing a problem where the player collision is fixed for only 1 direction
			//So if you use the simple integration here(playerPos += playerVelFrame * remainingTime) it migth slide against 
			//another wall and will not test for collision and will move through the walls while sliding into that direction.
			//This is where we call another CollisionFix() function so it doesn't grow through neither walls
			//Think about it, we only have 4 cases, where the player is sliding horizontally and it might encounter a wall on the
			//left or right side which will go through it or we might sliding vertically and it might encounter a wall on the
			//top or bottom side which will go through it.



			//At this point the player MUST BE moving horizontally, vertically or not moving at all(because we are sliding)
			//And if we encounter a wall and fix the collision we will not slide anymore(because we only go in cardinal directions)
			if (playerVel.x || playerVel.y)
			{
				CollisionFix(playerVel * DT, collidableBlocks, minCollisionTime = 1.0f - minCollisionTime, minNormal);
				//I reseted the values of minCollisionTime and minNormal because the function expects them to be so
				//At this point, the player should be moving against two walls at most
				//At 3 or 4 walls the velocities cancel each other out so still 2 walls at most
			}
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