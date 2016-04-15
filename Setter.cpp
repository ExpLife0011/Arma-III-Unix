#include "Drawer.h"

using namespace A2OA;

int  itemRef		= 0;

std::vector<DWORD> weapons;

void displayScripts( LPVOID args )
{
	
	D3D9_item*		item = (D3D9_item*)args;
	item->isEnabled = false;

	auto			world = World::Singleton();
	if (!world->getCameraOn()->getUnit()->getBase())
		return;

	auto			locPlayer	= world->getCameraOn()->getUnit()->getPlayer();
	auto			locUnit		= world->getCameraOn()->getUnit();

	if (!locPlayer->getBase())
		return;

	auto wpnID = locUnit->getWeaponID();
	cout << wpnID << endl;
}

void clearFile( LPVOID args ){
	D3D9_item*		item = (D3D9_item*) args;
	Memory*			m = (Memory*) item->arguments;

	m->clearFile();

	item->isEnabled = false;
}

void cleanUP( LPVOID args ){
	D3D9_item*		item = (D3D9_item*) args;
	Memory*			m = (Memory*) item->arguments;

	m->clearMemory();
	for( DWORD dwAddress : weapons ){
		m->write( dwAddress, 0 );
	}

	item->isEnabled = false;
}

void teleport( LPVOID args )
{
	D3D9_item*		item = (D3D9_item*) args;
	Memory*			m = (Memory*)item->arguments;

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();

	while( item->isEnabled )
	{
		if( GetAsyncKeyState( 0x57 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x + ( item->value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z + ( item->value*locPlayer->getViewY() );
			plyrPos.y = 0;

			locPlayer->setPos( plyrPos );
		} else if( GetAsyncKeyState( 0x53 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x - ( item->value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z - ( item->value*locPlayer->getViewY() );
			plyrPos.y = 0;

			locPlayer->setPos( plyrPos );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
	}
}

void teleportUnsafe( LPVOID args )
{
	D3D9_item*		item = (D3D9_item*) args;
	Memory*			m = (Memory*) item->arguments;

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();

	while( item->isEnabled )
	{
		if( GetAsyncKeyState( 0x57 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x + ( item->value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z + ( item->value*locPlayer->getViewY() );

			locPlayer->setPos( plyrPos );
		} else if( GetAsyncKeyState( 0x53 ) )
		{
			D3DXVECTOR3 plyrPos = locPlayer->getPos();

			plyrPos.x = plyrPos.x - ( item->value*locPlayer->getViewX() );
			plyrPos.z = plyrPos.z - ( item->value*locPlayer->getViewY() );

			locPlayer->setPos( plyrPos );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
	}
}

void unlockCar( LPVOID args ){
	D3D9_item*		item		= (D3D9_item*) args;
	Memory*			m			= (Memory*) item->arguments;

	item->isEnabled				= false;

	auto			world			= World::Singleton();
	auto			locPlayer		= world->getCameraOn()->getUnit()->getPlayer();
	auto			transData		= TransData::Singleton( );

	auto			entityTablePtr		= world->getEntityTable();
	auto			entityTable			= entityTablePtr->getTable();
	auto			entityTableSize		= entityTablePtr->getTableSize();

	for( DWORD i = 0; i < entityTableSize; i++ ){

		auto UnitInfo = entityTable->getUnitInfoById( i );
		auto Unit = UnitInfo->getUnit();
		auto UnitBase = Unit->getBase();

		if( UnitBase ){
			if( Unit->getID() == 0 ){

				auto VehicleInfo	= UnitInfo->getVehicle();
				auto Vehicle		= VehicleInfo->getVehicle();
				if( transData->Distance3D( locPlayer->getPos(), Vehicle->getPos() ) < 15 ){
					VehicleInfo->unlock();
					break;
				}
			}
		}

	}
}

void setDamage( LPVOID args ){
	D3D9_item*		item = (D3D9_item*) args;
	Memory*			m = (Memory*) item->arguments;

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();
	int				prevItemValue = 0;

	DWORD wID = locPlayer->getWeaponID(), wPtr = 0;
	DWORD offsets[ 3 ] = { ( wID * 0x24 + 0x4 ), 0x8, 0x200 };

	wPtr = m->readPointer<DWORD>( locPlayer->getBase() + 0x694, offsets, 3 );
	prevItemValue = (int) m->read<float>( wPtr + 0x140 );

	m->write( wPtr + 0x140, (float)item->value );
	item->value = prevItemValue;

	item->isEnabled = false;
}

void setIndirect( LPVOID args ){
	D3D9_item*		item = (D3D9_item*) args;
	Memory*			m = (Memory*) item->arguments;

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	DWORD wID = m->read<DWORD>( locPlayer->getBase() + 0x6E0 ), wPtr = 0;
	DWORD offsets[ 3 ] = { ( wID * 0x24 + 0x4 ), 0x8, 0x200 };

	wPtr = m->readPointer<DWORD>( locPlayer->getBase() + 0x694, offsets, 3 );
	m->write( wPtr + 0x144, (float)item->value );

	item->isEnabled = false;
}

void setRange( LPVOID args ){
	D3D9_item*		item = (D3D9_item*) args;
	Memory*			m = (Memory*) item->arguments;

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn( )->getUnit();

	DWORD wID = m->read<int>( locPlayer->getBase() + 0x6E0 ), wPtr = 0;
	DWORD offsets[ 3 ] = { ( wID * 0x24 + 0x4 ), 0x8, 0x200 };

	wPtr = m->readPointer<DWORD>( locPlayer->getBase() + 0x694, offsets, 3 );
	m->write( wPtr + 0x148, (float) item->value );

	item->isEnabled = false;
}

void resetStats( LPVOID args ){
	D3D9_item*		item = (D3D9_item*) args;
	Memory*			m = (Memory*) item->arguments;

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	DWORD wID = m->read<int>( locPlayer->getBase() + 0x6E0 ), wPtr = 0;
	DWORD offsets[ 3 ] = { ( wID * 0x24 + 0x4 ), 0x8, 0x200 };

	wPtr = m->readPointer<DWORD>( locPlayer->getBase() + 0x694, offsets, 3 );
	m->write( wPtr + 0x148, (float) 0 );
	m->write( wPtr + 0x144, (float) 0 );

	item->isEnabled = false;
}

void killPlayer( LPVOID args ){
	D3D9_item* item = (D3D9_item*) args;
	Memory* m = (Memory*) item->arguments;

	string			targetName			= item->itemVal;;	
	DWORD			targetBase			= 0;
	bool			isTargeted			= false;
	D3DXVECTOR3		targetPos			= D3DXVECTOR3( 0,0,0 );
	D3DXVECTOR3		targetAccel			= D3DXVECTOR3( 0,0,0 );

	while( item->isEnabled )
	{
		auto			world = World::Singleton();
		auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();
		auto			transData = TransData::Singleton( );

		auto			entityTablePtr = world->getEntityTable();
		auto			entityTable = entityTablePtr->getTable();
		auto			entityTableSize = entityTablePtr->getTableSize();

		auto			Munition = world->getMunition();
		auto			AmmonutionSize = Munition->getTableSize();
		auto			AmmonutionTable = Munition->getTable();

		auto			networkMgr = NetworkManager::Singleton();
		auto			scoreboard = networkMgr->getScoreboard();
		auto			scoreboardSize = scoreboard->getTableSize();
		auto			scoreboardTable = scoreboard->getScoreboardTable();

		isTargeted = false;
		for( DWORD i = 0; i < entityTableSize; i++ ){
			if( isTargeted )
				break;

			auto UnitInfo = entityTable->getUnitInfoById( i );
			auto Unit = UnitInfo->getUnit();
			auto UnitBase = Unit->getBase();
			if( UnitBase ){
				if( Unit->getID() > 1 ){ // PLAYER
					for( INT l = 0; l < scoreboardSize; l++ ){
						auto scoreboardEntity = scoreboardTable->getEntryById( l );
						if( scoreboardEntity->getID() == Unit->getID() ){
							if( scoreboardEntity->getString()->getString() == targetName ){
								targetBase	= UnitInfo->getBase();
								targetPos	= Unit->getPlayer()->getPos();
								targetAccel	= Unit->getPlayer()->getAccerelation();

								isTargeted = true;
								/* SETTING VECTOR TO HIT TARGET */
								
								D3DXVec3Normalize( &targetAccel, &targetAccel );
								D3DXVec3Subtract( &targetPos, &targetPos, &targetAccel );

								if (targetAccel != D3DXVECTOR3(0, 0, 0)) {
									D3DXVec3Scale( &targetAccel, &targetAccel, 1000.0f );
								} else {
									D3DXVec3Scale( &targetAccel, &D3DXVECTOR3(1.5f,1.5f,1.5f), 1000.0f );
								}

								isTargeted	= true;
								break;
								
							}
						}
					}
				} else if( Unit->getID() == 0 ){
					auto VehicleInfo = UnitInfo->getVehicle();
					auto Entity = VehicleInfo->getDriver();

					if( Entity->getBase() && Entity->getID() > 1 ){
						for( INT l = 0; l < scoreboardSize; l++ ){
							auto scoreboardEntity = scoreboardTable->getEntryById( l );
							if( scoreboardEntity->getID() == Entity->getID() ){
								if( scoreboardEntity->getString( )->getString( ) == targetName ){
									targetBase	= UnitInfo->getBase();
									targetPos	= VehicleInfo->getVehicle()->getPos();
									targetAccel	= VehicleInfo->getVehicle()->getAccerelation();

									isTargeted = true;
									/* SETTING VECTOR TO HIT TARGET */
									
									D3DXVec3Normalize(&targetAccel, &targetAccel);
									D3DXVec3Subtract(&targetPos, &targetPos, &targetAccel);

									if (targetAccel != D3DXVECTOR3(0, 0, 0)) {
										D3DXVec3Scale(&targetAccel, &targetAccel, 1000.0f);
									} else {
										D3DXVec3Scale(&targetAccel, &D3DXVECTOR3(1.5f, 1.5f, 1.5f), 1000.0f);
									}

									isTargeted = true;
									break;
									
								}
							}
						}
					}
				}
			}
		}

		if( !isTargeted || !targetBase ){ // IF CAN'T FIND TARGET ANYMORE
			item->value			= 0;
			item->isEnabled		= false;
			item->itemVal		= "NULL";

		//	setIndirect( args ); // SAFE PRECAUSIONS
		//	setRange( args ); // SAFE PRECAUSIONS

			break;
		}


		for( SIZE_T i = 0; i < AmmonutionSize; i++ ){
			auto Ammo			= AmmonutionTable->getAmmoById( i );
			if( !Ammo->getBase() )
				continue;

			auto AmmoStats		= Ammo->getStats();
			auto locAmmoStats	= m->read<DWORD>( world->getWorldBase() + 0x13A8 );

			if( AmmoStats == locAmmoStats ){ // LOCAL PLAYERS BULLET
				int reference = m->read<int>( targetBase );
				
				Ammo->setPos( targetPos );
				Ammo->setAcceleration( targetAccel );

				m->write(Ammo->getBase() + 0x1FC, targetBase);
				m->write(targetBase, (reference + 1));
			}
		}
	}
}

void framePlayer( LPVOID args ){
	D3D9_item* item = (D3D9_item*) args;
	Memory* m = (Memory*) item->arguments;

	string			targetName = item->itemVal;
	DWORD			targetBase = 0;
	bool			isTargeted = false;

	while( item->isEnabled )
	{
		auto			world = World::Singleton();
		auto			locPlayer = world->getCameraOn()->getUnit()->getPlayer();
		auto			transData = TransData::Singleton();

		auto			entityTablePtr = world->getEntityTable();
		auto			entityTable = entityTablePtr->getTable();
		auto			entityTableSize = entityTablePtr->getTableSize();

		auto			Munition = world->getMunition();
		auto			AmmonutionSize = Munition->getTableSize();
		auto			AmmonutionTable = Munition->getTable();

		auto			networkMgr = NetworkManager::Singleton();
		auto			scoreboard = networkMgr->getScoreboard();
		auto			scoreboardSize = scoreboard->getTableSize();
		auto			scoreboardTable = scoreboard->getScoreboardTable();

		isTargeted = false;
		for( DWORD i = 0; i < entityTableSize; i++ ){
			if( isTargeted )
				break;

			auto UnitInfo = entityTable->getUnitInfoById( i );
			auto Unit = UnitInfo->getUnit();
			auto UnitBase = Unit->getBase();
			if( UnitBase ){
				if( Unit->getID() > 1 ){ // PLAYER
					for( INT l = 0; l < scoreboardSize; l++ ){
						auto scoreboardEntity = scoreboardTable->getEntryById( l );
						if( scoreboardEntity->getID() == Unit->getID() ){
							if( scoreboardEntity->getString()->getString() == targetName ){

								targetBase = UnitInfo->getBase();
								isTargeted = true;
								break;
							}
						}
					}
				} else if( Unit->getID() == 0 ){
					auto VehicleInfo = UnitInfo->getVehicle();
					auto Entity = VehicleInfo->getDriver();

					if( Entity->getBase() && Entity->getID() > 1 ){
						for( INT l = 0; l < scoreboardSize; l++ ){
							auto scoreboardEntity = scoreboardTable->getEntryById( l );
							if( scoreboardEntity->getID() == Entity->getID() ){
								if( scoreboardEntity->getString()->getString() == targetName ){
									targetBase = UnitInfo->getBase();
									isTargeted = true;
									break;
								}
							}
						}
					}
				}
			}
		}

		if( !isTargeted || !targetBase ){ // IF CAN'T FIND TARGET ANYMORE
			item->value = 0;
			item->isEnabled = false;
			item->itemVal = "NULL";

			break;
		}


		for( SIZE_T i = 0; i < AmmonutionSize; i++ ){
			auto Ammo = AmmonutionTable->getAmmoById( i );
			if( !Ammo->getBase() )
				continue;

			auto AmmoStats = Ammo->getStats();
			auto locAmmoStats = m->read<DWORD>( world->getWorldBase() + 0x13A8 );

			if( AmmoStats == locAmmoStats ){ // LOCAL PLAYERS BULLET
				/*
				auto  plyrPos = locPlayer->getPos();
				auto  bltPos = Ammo->getPos();

				double distance = sqrt( ( plyrPos.x * bltPos.x + plyrPos.y * bltPos.y + plyrPos.z * bltPos.z ) );
				if( distance > 1.7 ){
				*/
					int reference = m->read<int>( targetBase );

					m->write( Ammo->getBase() + 0x1FC, targetBase );
					m->write( targetBase, ( reference + 1 ) );
			}
		}


		//std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
	}
}

void spawnWeapon( LPVOID args ){
	D3D9_item* item = (D3D9_item*) args;
	Memory* m = (Memory*) item->arguments;

	auto			WeaponTable		= WeaponTable::Singleton();
	auto			WeaponTableSize = WeaponTable->getTableSize();

	auto			world			= World::Singleton();
	auto			locPlayer		= world->getCameraOn()->getUnit();

	auto			InventoryTable			= locPlayer->getInventoryTable();
	auto			InventoryTableSize		= InventoryTable->getTableSize();
	auto			InventoryTableMaxSize	= InventoryTable->getTableMaxSize();
	auto			Inventory				= InventoryTable->getInventory( );

	if( item->value > (int)(WeaponTableSize - 1) ){
		item->value = 0;
		item->isEnabled = false;
		return;
	}

	auto  Weapon = WeaponTable->getEntryById( item->value );
	if( Weapon->getBase() ){
		if( InventoryTableSize < InventoryTableMaxSize ){
			DWORD distance = Inventory->calculateDistance( InventoryTableSize );

			m->write( distance, Weapon->getBase() );
			InventoryTable->setTableSize( InventoryTableSize + 1 );

			weapons.push_back( distance );
		}
	}

	item->isEnabled = false;
}

void spawnAmmunation( LPVOID args ){
	D3D9_item* item = (D3D9_item*) args;
	Memory* m = (Memory*) item->arguments;

	auto			ConsumableTable = ConsumableTable::Singleton();
	auto			ConsumableTableSize = ConsumableTable->getTableSize();

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	auto			InventoryTable = locPlayer->getInventoryTable();
	auto			InventoryTableSize = InventoryTable->getConsumableTableSize();
	auto			InventoryTableMaxSize = InventoryTable->getConsumableTableMaxSize( );
	auto			Inventory = InventoryTable->getConsumableInventory();

	if( item->value > (int)(ConsumableTableSize - 1) ){
		item->value = 0;
		item->isEnabled = false;
		return;
	}

	auto  Consumable = ConsumableTable->getEntryById( item->value );
	if( Consumable->getBase() ){
		if( InventoryTableSize < InventoryTableMaxSize ){
			DWORD memory = m->allocateMemory( 0x2C );
			if (!memory) {
				console->sendInput( "Failed to allocate memory!" );
				return;
			}
				

			DWORD item1 = m->read<DWORD>( m->read<DWORD>( Inventory->getBase( ) + 0x28 ) );
			DWORD item2 = m->read<DWORD>( m->read<DWORD>( Inventory->getBase( ) + 0x4 ) );

			m->write( memory, m->read<DWORD>( Inventory->getBase() ) );
			m->write( memory + 0x4, item2 );
			m->write( memory + 0x8, Consumable->getBase() );

			m->write( memory + 0x28, item1 );
			m->write( memory + 0x2C, itemRef );
			itemRef++;

			/* UNCHECKED */
			int ammoXOR = m->read<DWORD>( Consumable->getBase() + 0x2C ) ^ (int)( 0xBABAC8B6L );
			int ammoVal = ammoXOR << 1;

			m->write( memory + 0xC, int( ammoXOR - ammoVal ) );
			m->write( memory + 0x24, ammoVal );
			/* END UNCHECKED */

			DWORD distance = Inventory->calculateDistance( InventoryTableSize );

			m->write( distance, memory );
			InventoryTable->setConsumableTableSize( InventoryTableSize + 1 );
		}
	}

	item->isEnabled = false;
}

void spawnWeaponConsole( int id, int amount )
{
	if( id <= 0 || amount <= 0 )
		return;

	auto			WeaponTable				= WeaponTable::Singleton();
	auto			WeaponTableSize			= WeaponTable->getTableSize();

	auto			world					= World::Singleton();
	auto			locPlayer				= world->getCameraOn()->getUnit();

	auto			InventoryTable			= locPlayer->getInventoryTable();
	auto			InventoryTableSize		= InventoryTable->getTableSize();
	auto			InventoryTableMaxSize	= InventoryTable->getTableMaxSize();
	auto			Inventory				= InventoryTable->getInventory();

	auto  Weapon = WeaponTable->getEntryById( id );
	if( Weapon->getBase() ){

		for( int i = 0; i < amount; i++ )
		{
			if( InventoryTableSize < InventoryTableMaxSize ){
				DWORD distance = Inventory->calculateDistance( InventoryTableSize );

				m->write( distance, Weapon->getBase() );
				InventoryTable->setTableSize( InventoryTableSize + 1 );
				InventoryTableSize++;

				weapons.push_back( distance );
			}
		}
	}
}

void spawnConsumableConsole( int id, int amount )
{
	if( id <= 0 || amount <= 0 )
		return;

	auto			ConsumableTable = ConsumableTable::Singleton();
	auto			ConsumableTableSize = ConsumableTable->getTableSize();

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	auto			InventoryTable = locPlayer->getInventoryTable();
	auto			InventoryTableSize = InventoryTable->getConsumableTableSize();
	auto			InventoryTableMaxSize = InventoryTable->getConsumableTableMaxSize();
	auto			Inventory = InventoryTable->getConsumableInventory();

	auto  Consumable = ConsumableTable->getEntryById( id );
	if( Consumable->getBase() ){

		for( int i = 0; i < amount; i++ )
		{
			if( InventoryTableSize < InventoryTableMaxSize ){
				DWORD memory = m->allocateMemory( 0x2C );

				DWORD item1 = m->read<DWORD>( m->read<DWORD>( Inventory->getBase() + 0x28 ) );
				DWORD item2 = m->read<DWORD>( m->read<DWORD>( Inventory->getBase() + 0x4 ) );

				m->write( memory, m->read<DWORD>( Inventory->getBase() ) );
				m->write( memory + 0x4, item2 );
				m->write( memory + 0x8, Consumable->getBase() );

				m->write( memory + 0x28, item1 );
				m->write( memory + 0x2C, itemRef );
				itemRef++;

				/* UNCHECKED */
				int ammoXOR = m->read<DWORD>( Consumable->getBase() + 0x2C ) ^ (int) ( 0xBABAC8B6L );
				int ammoVal = ammoXOR << 1;

				m->write( memory + 0xC, int( ammoXOR - ammoVal ) );
				m->write( memory + 0x24, ammoVal );
				/* END UNCHECKED */

				DWORD distance = Inventory->calculateDistance( InventoryTableSize );

				m->write( distance, memory );
				InventoryTable->setConsumableTableSize( InventoryTableSize + 1 );
				InventoryTableSize++;
			}
		}
	}
}

void listWeapon( string data )
{
	auto			WeaponTable = WeaponTable::Singleton();
	auto			WeaponTableSize = WeaponTable->getTableSize();

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	auto			InventoryTable = locPlayer->getInventoryTable();
	auto			InventoryTableSize = InventoryTable->getTableSize();
	auto			InventoryTableMaxSize = InventoryTable->getTableMaxSize();
	auto			Inventory = InventoryTable->getInventory();


	for( size_t i = 0; i < ( WeaponTableSize - 1 ); i++ )
	{
		auto		Consumable = WeaponTable->getEntryById( i );
		if( Consumable->getBase() )
		{
			string typeName			= Consumable->getType()->getString();
			string typeNameLower	= typeName;

			if( typeName.length() > 0 )
			{
				for( size_t l = 0; l < data.length(); l++ )
				{
					data[ l ] = tolower( data[ l ] );
				}
				for( size_t l = 0; l < typeName.length(); l++ )
				{
					typeNameLower[ l ] = tolower( typeName[ l ] );
				}

				if( data == "all" || typeNameLower.find( data, 0 ) != -1 )
				{
					cout << "ID: " << i << " Type: " << typeName << " Name: " << Consumable->getName()->getString() << endl;
				}
			}
		}
	}

}

void listConsumable( string data )
{
	auto			ConsumableTable				= ConsumableTable::Singleton();
	auto			ConsumableTableSize			= ConsumableTable->getTableSize();

	auto			world						= World::Singleton();
	auto			locPlayer					= world->getCameraOn()->getUnit();

	auto			InventoryTable				= locPlayer->getInventoryTable();
	auto			InventoryTableSize			= InventoryTable->getConsumableTableSize();
	auto			InventoryTableMaxSize		= InventoryTable->getConsumableTableMaxSize();
	auto			Inventory					= InventoryTable->getConsumableInventory();


	for( size_t i = 0; i < ( ConsumableTableSize - 1 ); i++ )
	{
		auto		Consumable = ConsumableTable->getEntryById( i );
		if( Consumable->getBase() )
		{
			string typeName = Consumable->getType()->getString();
			string typeNameLower = typeName;

			if( typeName.length() > 0 )
			{
				for( size_t l = 0; l < data.length(); l++ )
				{
					data[ l ] = tolower( data[ l ] );
				}
				for( size_t l = 0; l < typeName.length( ); l++ )
				{
					typeNameLower[ l ] = tolower( typeName[ l ] );
				}

				if( data == "all" || typeNameLower.find( data, 0 ) != -1 )
				{
					cout << "ID: " << i << " Type: " << typeName << " Name: " << Consumable->getName( )->getString( ) << endl;
				}
			}
		}
	}

}

void findConsumable( string data )
{
	auto			ConsumableTable = ConsumableTable::Singleton();
	auto			ConsumableTableSize = ConsumableTable->getTableSize();

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	auto			InventoryTable = locPlayer->getInventoryTable();
	auto			InventoryTableSize = InventoryTable->getConsumableTableSize();
	auto			InventoryTableMaxSize = InventoryTable->getConsumableTableMaxSize();
	auto			Inventory = InventoryTable->getConsumableInventory();


	for( size_t i = 0; i < ( ConsumableTableSize - 1 ); i++ )
	{
		auto		Consumable = ConsumableTable->getEntryById( i );
		if( Consumable->getBase() )
		{
			string itemName			= Consumable->getName()->getString();
			string itemNameLower	= itemName;

			if( itemName.length() > 0 )
			{
				for( size_t l = 0; l < itemName.length(); l++ )
				{
					itemNameLower[ l ] = (char) tolower( itemName[ l ] );
				}
				
				for( size_t l = 0; l < data.length(); l++ )
				{
					data[ l ] = (char)tolower( data[ l ] );
				}

				if( itemNameLower.find( data, 0 ) != -1 )
				{
					cout << "ID: " << i << " Type: " << Consumable->getType()->getString() << " Name: " << itemName << endl;
				}
			}
		}
	}

}

void findWeapon( string data )
{
	auto			WeaponTable = WeaponTable::Singleton();
	auto			WeaponTableSize = WeaponTable->getTableSize();

	auto			world = World::Singleton();
	auto			locPlayer = world->getCameraOn()->getUnit();

	auto			InventoryTable = locPlayer->getInventoryTable();
	auto			InventoryTableSize = InventoryTable->getTableSize();
	auto			InventoryTableMaxSize = InventoryTable->getTableMaxSize();
	auto			Inventory = InventoryTable->getInventory();


	for( size_t i = 0; i < ( WeaponTableSize - 1 ); i++ )
	{
		auto		Consumable = WeaponTable->getEntryById( i );
		if( Consumable->getBase() )
		{
			string itemName = Consumable->getName()->getString();
			string itemNameLower = itemName;

			if( itemName.length() > 0 )
			{
				for( size_t l = 0; l < itemName.length(); l++ )
				{
					itemNameLower[ l ] = tolower( itemName[ l ] );
				}
				for( size_t l = 0; l < data.length(); l++ )
				{
					data[ l ] = tolower( data[ l ] );
				}

				if( itemNameLower.find( data, 0 ) != -1 )
				{
					cout << "ID: " << i << " Type: " << Consumable->getType( )->getString( ) << " Name: " << itemName << endl;
				}
			}
		}
	}

}

///////////////////////////////////////////////////////////////
///////             D3D9 SETTER FUNCTIONS          ///////////
//////////////////////////////////////////////////////////////

void setPlayer( LPVOID args ){
	D3D9_textBox* textBox = (D3D9_textBox*)args;
	D3D9_item* item = (D3D9_item*)textBox->item;
	Memory* m = (Memory*) item->arguments;

	auto			networkMgr = NetworkManager::Singleton();
	auto			scoreboard = networkMgr->getScoreboard();
	auto			scoreboardSize = scoreboard->getTableSize();
	auto			scoreboardTable = scoreboard->getScoreboardTable();

	if( textBox->side == "LEFT" )
	{
		if( textBox->step > 0 ){
			textBox->step--;
		} else {
			textBox->step = (int)(scoreboardSize - 1);
		}
			
	} else if( textBox->side == "RIGHT" )
		textBox->step++;

	if( textBox->step > (int) (scoreboardSize - 1) ){
		textBox->step = 0;
	}

	auto			scoreboardEntity = scoreboardTable->getEntryById( textBox->step );
	item->itemVal = scoreboardEntity->getString()->getString();
}

void setWeapon( LPVOID args ){
	D3D9_textBox* textBox = (D3D9_textBox*) args;
	D3D9_item* item = (D3D9_item*) textBox->item;
	Memory* m = (Memory*) item->arguments;

	if( item->isEnabled )
		return;

	auto WeaponTable		= WeaponTable::Singleton();
	auto WeaponTableSize	= WeaponTable->getTableSize();

	if( textBox->side == "LEFT" )
	{
		if( textBox->step > 0 ){
			textBox->step--;
		} else {
			textBox->step = (WeaponTableSize - 1);
		}	
	} else if( textBox->side == "RIGHT" ){
		if( textBox->step > (int)(WeaponTableSize - 1) ){
			textBox->step = 0;
		} else {
			textBox->step++;
		}
	}
		
	auto Weapon				= WeaponTable->getEntryById( textBox->step );

	if( !Weapon->getType()->getString().compare( "rifle" ) ){
		textBox->item->itemVal = Weapon->getName( )->getString( );
		textBox->item->value   = textBox->step;
	} else {
		setWeapon( args );
	}
		
}

void setAmmunation( LPVOID args ){
	D3D9_textBox* textBox = (D3D9_textBox*) args;
	D3D9_item* item = (D3D9_item*) textBox->item;
	Memory* m = (Memory*) item->arguments;

	if( item->isEnabled )
		return;

	auto ConsumableTable		= ConsumableTable::Singleton( );
	auto ConsumableTableSize	= ConsumableTable->getTableSize();

	if( textBox->side == "LEFT" )
	{
		if( textBox->step > 0 ){
			textBox->step--;
		} else {
			textBox->step = ( ConsumableTableSize - 1 );
		}
	} else if( textBox->side == "RIGHT" ){
		if( textBox->step > (int) ( ConsumableTableSize - 1 ) ){
			textBox->step = 0;
		} else {
			textBox->step++;
		}
	}

	auto Consumable = ConsumableTable->getEntryById( textBox->step );

	if( Consumable->getType()->getLength() > 0 ){
		if( Consumable->getName()->getLength() > 0 ){
			if( !Consumable->getType()->getString().compare( "magazine" ) ){
				textBox->item->itemVal = Consumable->getName()->getString();
				textBox->item->value = textBox->step;
			} else {
				setConsumable( args );
			}
		} else {
			setConsumable( args );
		}
	} else {
		setConsumable( args );
	}
}

void setConsumable( LPVOID args ){
	D3D9_textBox* textBox = (D3D9_textBox*) args;
	D3D9_item* item = (D3D9_item*) textBox->item;
	Memory* m = (Memory*) item->arguments;

	if( item->isEnabled )
		return;

	auto ConsumableTable = ConsumableTable::Singleton();
	auto ConsumableTableSize = ConsumableTable->getTableSize();

	if( textBox->side == "LEFT" )
	{
		if( textBox->step > 0 ){
			textBox->step--;
		} else {
			textBox->step = ( ConsumableTableSize - 1 );
		}
	} else if( textBox->side == "RIGHT" ){
		if( textBox->step > (int) ( ConsumableTableSize - 1 ) ){
			textBox->step = 0;
		} else {
			textBox->step++;
		}
	}

	auto Consumable = ConsumableTable->getEntryById( textBox->step );

	if( Consumable->getType()->getLength() > 0 ){
		if( Consumable->getName()->getLength() > 0 ){
			if( Consumable->getType()->getString().compare( "magazine" ) ){
				textBox->item->itemVal = Consumable->getName()->getString();
				textBox->item->value = textBox->step;
			} else {
				setConsumable( args );
			}
		} else {
			setConsumable( args );
		}
	} else {
		setConsumable( args );
	}
}