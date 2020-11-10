/*
	This software is distributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty

	Created by Kyrylo Sovailo (github.com/Meta-chan, k.sovailo@gmail.com)
	Reinventing bicycles since 2020
*/

#ifndef P6_CONSTRUCTION
#define P6_CONSTRUCTION

#include "p6_material.hpp"
#include <vector>

namespace p6
{
	struct Construction
	{
	private:
		struct Header
		{
			char signature[8] = { 'P','6', 'C', 'N', 'S', 'T', '0', '\0'};
			uint node;
			uint stick;
			uint force;
			uint material;
		};

		struct StaticNode
		{
			bool free;
			Coord coord;
		};

		struct Node : StaticNode
		{
			Coord coord_simulated;
		};

		struct Stick
		{
			uint node[2];
			uint material;
			real area;
		};

		struct Force
		{
			uint node;
			Coord direction;
		};

		std::vector<Node> _node;
		std::vector<Stick> _stick;
		std::vector<Force> _force;
		std::vector<Material*> _material;
		bool _simulation;

	public:
		//Node
		uint create_node(Coord coord, bool free);
		void delete_node(uint node);
		void set_node_coord(uint node, Coord coord);
		void set_node_free(uint node, bool free);
		uint get_node_count()							const;
		Coord get_node_coord(uint node)					const;
		bool get_node_free(uint node)					const;
		
		//Stick
		uint create_stick(const uint node[2], uint material, real area);
		void delete_stick(uint stick);
		void set_stick_material(uint stick, uint material);
		void set_stick_area(uint stick, real area);
		uint get_stick_count()							const;
		uint get_stick_material(uint stick)				const;
		real get_stick_area(uint stick)					const;
		void get_stick_node(uint stick, uint node[2])	const;
		real get_stick_length(uint stick)				const;
		real get_stick_strain(uint stick)				const;
		real get_stick_force(uint stick)				const;

		//Force
		uint create_force(uint node, Coord direction);
		void delete_force(uint force);
		void set_force_direction(uint force, Coord coord);
		uint get_force_count()							const;
		Coord get_force_direction(uint force)			const;
		uint get_force_node(uint force)					const;

		//Material
		uint create_linear_material(const String name, real modulus);
		uint create_nonlinear_material(const String name, const String formula);
		void delete_material(uint material);
		uint get_material_count()						const;
		String get_material_name(uint material)			const;
		Material::Type get_material_type(uint material)	const;
		real get_material_modulus(uint material)		const;
		String get_material_formula(uint material)		const;
		
		//Maintanance
		void save(const String filepath)				const;
		void load(const String filepath);
		void import(const String filepath);
		void simulate(bool sim);
		bool simulation()								const;
		~Construction();
	};
}

#endif	//#ifndef P6