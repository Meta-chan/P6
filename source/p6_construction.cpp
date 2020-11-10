/*
	This software is distributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty

	Created by Kyrylo Sovailo (github.com/Meta-chan, k.sovailo@gmail.com)
	Reinventing bicycles since 2020
*/

#include "../header/p6_construction.hpp"
#include "../header/p6_indexer.hpp"
#include "../header/p6_linear_material.hpp"
#include "../header/p6_nonlinear_material.hpp"
#include <wx/file.h>
#include <Eigen>
#include <cassert>
#include <cmath>

p6::uint p6::Construction::create_node(Coord coord, bool free)
{
	assert(!_simulation);
	assert(coord.x == coord.x);
	assert(coord.y == coord.y);
	Node node;
	node.free = free;
	node.coord = coord;
	_node.push_back(node);
	return _node.size() - 1;
}

void p6::Construction::delete_node(uint node)
{
	assert(!_simulation);
	for (uint i = _stick.size() - 1; i != (uint)-1; i--)
	{
		if (_stick[i].node[0] == node || _stick[i].node[1] == node)
		{
			_stick.erase(_stick.cbegin() + i);
		}
		else
		{
			if (_stick[i].node[0] > node) _stick[i].node[0]--;
			if (_stick[i].node[1] > node) _stick[i].node[1]--;
		}
	}
	_node.erase(_node.cbegin() + node);
}

void p6::Construction::set_node_coord(uint node, Coord coord)
{
	assert(!_simulation);
	assert(coord.x == coord.x);
	assert(coord.y == coord.y);
	_node[node].coord = coord;
}

void p6::Construction::set_node_free(uint node, bool free)
{
	assert(!_simulation);
	_node[node].free = free;
}

p6::uint p6::Construction::get_node_count() const
{
	return _node.size();
}

p6::Coord p6::Construction::get_node_coord(uint node) const
{
	return _simulation ? _node[node].coord_simulated : _node[node].coord;
}

bool p6::Construction::get_node_free(uint node) const
{
	return _node[node].free;
}

p6::uint p6::Construction::create_stick(const uint node[2], uint material, real area)
{
	assert(!_simulation);
	assert(area >= 0);
	assert(node[0] != node[1]);
	assert(node[0] < _node.size());
	assert(node[1] < _node.size());
	for (uint i = 0; i < _stick.size(); i++)
	{
		assert(!((_stick[i].node[0] == node[0] && _stick[i].node[0] == node[1])
			&& (_stick[i].node[1] == node[1] && _stick[i].node[1] == node[1])));
	}

	Stick stick;
	stick.node[0] = node[0];
	stick.node[1] = node[1];
	stick.material = material;
	stick.area = area;
	_stick.push_back(stick);
	return _stick.size() - 1;
}

void p6::Construction::delete_stick(uint stick)
{
	assert(!_simulation);
	_stick.erase(_stick.cbegin() + stick);
}

void p6::Construction::set_stick_material(uint stick, uint material)
{
	assert(!_simulation);
	_stick[stick].material = material;
}

void p6::Construction::set_stick_area(uint stick, real area)
{
	assert(!_simulation);
	assert(area == area);
	_stick[stick].area = area;
}

p6::uint p6::Construction::get_stick_count() const
{
	return _stick.size();
}

p6::uint p6::Construction::get_stick_material(uint stick) const
{
	return _stick[stick].material;
}

p6::real p6::Construction::get_stick_area(uint stick) const
{
	return _stick[stick].area;
}

void p6::Construction::get_stick_node(uint stick, uint node[2]) const
{
	node[0] = _stick[stick].node[0];
	node[1] = _stick[stick].node[1];
}

p6::real p6::Construction::get_stick_length(uint stick) const
{
	const Node *node[2] = { &_node[_stick[stick].node[0]], &_node[_stick[stick].node[1]] };
	return _simulation ?
		node[0]->coord_simulated.distance(node[1]->coord_simulated) :
		node[0]->coord.distance(node[1]->coord);
}

p6::real p6::Construction::get_stick_strain(uint stick) const
{
	assert(_simulation);
	const Node *node[2] = { &_node[_stick[stick].node[0]], &_node[_stick[stick].node[1]] };
	return (
		node[0]->coord_simulated.distance(node[1]->coord_simulated) /
		node[0]->coord.distance(node[1]->coord)
		) - 1.0;
}

p6::real p6::Construction::get_stick_force(uint stick) const
{
	assert(_simulation);
	return _stick[stick].area * _material[_stick[stick].material]->stress(get_stick_strain(stick));
}

p6::uint p6::Construction::create_force(uint node, Coord direction)
{
	assert(!_simulation);
	assert(direction.x == direction.x);
	assert(direction.y == direction.y);
	assert(node < _node.size());
	Force force;
	force.node = node;
	force.direction = direction;
	_force.push_back(force);
	return _force.size() - 1;
}

void p6::Construction::delete_force(uint force)
{
	assert(!_simulation);
	_force.erase(_force.cbegin() + force);
}

void p6::Construction::set_force_direction(uint force, Coord direction)
{
	assert(!_simulation);
	assert(direction.x == direction.x);
	assert(direction.y == direction.y);
	_force[force].direction = direction;
}

p6::uint p6::Construction::get_force_count() const
{
	return _force.size();
}

p6::Coord p6::Construction::get_force_direction(uint force) const
{
	return _force[force].direction;
}

p6::uint p6::Construction::get_force_node(uint force) const
{
	return _force[force].node;
}

p6::uint p6::Construction::create_linear_material(const String name, real modulus)
{
	assert(!_simulation);
	for (uint i = 0; i < _material.size(); i++)
	{
		if (name == _material[i]->name())
		{
			Material *material = new LinearMaterial(name, modulus);
			delete _material[i];
			_material[i] = material;
			return i;
		}
	}
	_material.push_back(new LinearMaterial(name, modulus));
	return _material.size() - 1;
}

p6::uint p6::Construction::create_nonlinear_material(const String name, const String formula)
{
	assert(!_simulation);
	for (uint i = 0; i < _material.size(); i++)
	{
		if (name == _material[i]->name())
		{
			Material *material = new NonlinearMaterial(name, formula);
			delete _material[i];
			_material[i] = material;
			return i;
		}
	}

	_material.push_back(new NonlinearMaterial(name, formula));
	return _material.size() - 1;
}

void p6::Construction::delete_material(uint material)
{
	assert(!_simulation);
	for (uint i = 0; i < _stick.size(); i++)
	{
		if (_stick[i].material == material) _stick[i].material = (uint)-1;
	}
	delete _material[material];
	_material.erase(_material.cbegin() + material);
}

p6::uint p6::Construction::get_material_count() const
{
	return _material.size();
}

p6::String p6::Construction::get_material_name(uint material) const
{
	return _material[material]->name();
}

p6::Material::Type p6::Construction::get_material_type(uint material) const
{
	return _material[material]->type();
}

p6::real p6::Construction::get_material_modulus(uint material) const
{
	assert(_material[material]->type() == Material::Type::linear);
	return ((LinearMaterial*)_material[material])->modulus();
}

p6::String p6::Construction::get_material_formula(uint material) const
{
	assert(_material[material]->type() == Material::Type::nonlinear);
	return ((NonlinearMaterial*)_material[material])->formula();
}

void p6::Construction::save(const String filepath) const
{
	//Open file
	wxFile file(filepath, wxFile::OpenMode::write);
	if (!file.IsOpened()) throw std::runtime_error("File cannot be opened for write");
	Header header;
	header.node = _node.size();
	header.stick = _stick.size();
	header.force = _force.size();
	header.material = _material.size();
	file.Write(&header, sizeof(Header));

	//Nodes
	for (uint i = 0; i < _node.size(); i++)
	{
		file.Write(&_node[i], sizeof(StaticNode));
	}

	//Sticks
	for (uint i = 0; i < _stick.size(); i++)
	{
		file.Write(&_stick[i], sizeof(Stick));
	}

	//Forces
	for (uint i = 0; i < _force.size(); i++)
	{
		file.Write(&_force[i], sizeof(Force));
	}

	//Materials
	for (uint i = 0; i < _material.size(); i++)
	{
		//Name
		String name = _material[i]->name();
		uint len = name.size();
		file.Write(&len, sizeof(uint));
		file.Write(name.data(), len);

		//Type
		Material::Type type = _material[i]->type();
		file.Write(&type, sizeof(Material::Type));

		if (type == Material::Type::linear)
		{
			//Modulus
			real modulus = ((LinearMaterial*)_material[i])->modulus();
			file.Write(&modulus, sizeof(real));
		}
		else
		{
			//Formula
			String formula = ((NonlinearMaterial*)_material[i])->formula();
			len = formula.size();
			file.Write(&len, sizeof(uint));
			file.Write(formula.data(), len);
		}
	}
}

void p6::Construction::load(const String filepath)
{
	//Open file
	wxFile file(filepath, wxFile::OpenMode::read);
	if (!file.IsOpened()) throw std::runtime_error("File cannot be opened for read");
	Header header, sample;
	file.Read(&header, sizeof(Header));
	if (memcmp(header.signature, sample.signature, 8) != 0) throw ("Invalid file format");
	
	//Nodes
	_node.resize(header.node);
	for (uint i = 0; i < _node.size(); i++)
	{
		file.Read(&_node[i], sizeof(StaticNode));
	}

	//Sticks
	_stick.resize(header.stick);
	for (uint i = 0; i < _stick.size(); i++)
	{
		file.Read(&_stick[i], sizeof(Stick));
	}

	//Forces
	_force.resize(header.force);
	for (uint i = 0; i < _force.size(); i++)
	{
		file.Read(&_force[i], sizeof(Force));
	}

	//Materials
	for (uint i = 0; i < _material.size(); i++) delete _material[i];
	_material.resize(header.material);
	for (uint i = 0; i < _material.size(); i++)
	{
		//Name
		uint len;
		file.Read(&len, sizeof(uint));
		String name(len, '\0');
		file.Read(&name[0], len);

		//Type
		Material::Type type;
		file.Read(&type, sizeof(Material::Type));

		if (type == Material::Type::linear)
		{
			//Modulus
			real modulus;
			file.Read(&modulus, sizeof(real));
			_material[i] = new LinearMaterial(name, modulus);
		}
		else
		{
			//Formula
			file.Read(&len, sizeof(uint));
			String formula(len, '\0');
			file.Read(&formula[0], len);
			_material[i] = new NonlinearMaterial(name, formula);
		}
	}
}

void p6::Construction::import(const String filepath)
{
	//Open file
	wxFile file(filepath, wxFile::OpenMode::read);
	if (!file.IsOpened()) throw std::runtime_error("File cannot be opened for read");
	Header header, sample;
	file.Read(&header, sizeof(Header));
	if (memcmp(header.signature, sample.signature, 8) != 0) throw ("Invalid file format");

	uint old_node_size = _node.size();
	uint old_stick_size = _stick.size();
	uint old_force_size = _force.size();
	uint old_material_size = _material.size();

	//Nodes
	_node.resize(old_node_size + header.node);
	for (uint i = old_node_size; i < _node.size(); i++)
	{
		file.Read(&_node[i], sizeof(StaticNode));
	}

	//Sticks
	_stick.resize(old_stick_size + header.stick);
	for (uint i = old_stick_size; i < _stick.size(); i++)
	{
		file.Read(&_stick[i], sizeof(Stick));
		_stick[i].node[0] += old_node_size;
		_stick[i].node[1] += old_node_size;
		_stick[i].material += old_material_size;
	}

	//Forces
	_force.resize(old_force_size + header.force);
	for (uint i = old_force_size; i < _force.size(); i++)
	{
		file.Read(&_force[i], sizeof(Force));
		_force[i].node += old_node_size;
	}

	//Materials
	for (uint i = 0; i < header.material; i++)
	{
		//Name
		uint len;
		file.Read(&len, sizeof(uint));
		String name(len, '\0');
		file.Read(&name[0], len);

		//Find existing material
		uint existing_material = (uint)-1;
		for (uint j = 0; j < old_material_size; j++)
		{
			if (_material[j]->name() == name)
			{
				existing_material = j;
				break;
			}
		}

		//Type
		Material::Type type;
		file.Read(&type, sizeof(Material::Type));

		//Creating material
		Material *new_material;
		if (type == Material::Type::linear)
		{
			//Modulus
			real modulus;
			file.Read(&modulus, sizeof(real));
			if (existing_material != (uint)-1) new_material = new LinearMaterial(name, modulus);
		}
		else
		{
			//Formula
			file.Read(&len, sizeof(uint));
			String formula(len, '\0');
			file.Read(&formula[0], len);
			if (existing_material != (uint)-1) new_material = new NonlinearMaterial(name, formula);
		}

		//Adding material to materials
		if (existing_material == (uint)-1)
		{
			_material.push_back(new_material);
			existing_material = _material.size() - 1;
		}

		//Correcting sticks
		for (uint i = old_stick_size; i < _stick.size(); i++)
		{
			if (_stick[i].material == old_material_size + i) _stick[i].material = existing_material;
		}
	}
}

void p6::Construction::simulate(bool sim)
{
	if (sim == _simulation) return;
	else if (!sim) { _simulation = false; return; }

	//Initial lengths of sticks
	std::vector<real> lengths(_stick.size());
	for (uint i = 0; i < _stick.size(); i++) lengths[i] = get_stick_length(i);

	//Creating general-to-free and free-to general maps
	std::vector<uint> free_to_node;
	std::vector<uint> node_to_free;
	{
		uint nfree = 0;
		node_to_free.resize(_node.size(), (uint)-1);
		for (uint i = 0; i < _node.size(); i++)
		{
			if (_node[i].free)
			{
				node_to_free[i] = nfree;
				nfree++;
			}
		}

		free_to_node.reserve(nfree);
		for (uint i = 0; i < _node.size(); i++)
		{
			if (_node[i].free)
			{
				free_to_node.push_back(i);
			}
		}
	}

	//Creating indexer
	Indexer r(free_to_node.size(), _stick.size());

	//Creating state vector
	Eigen::Vector<real, Eigen::Dynamic> s(r.variable_number());
	for (uint i = 0; i < free_to_node.size(); i++)
	{
		s(r.node_variable_x(i)) = _node[free_to_node[i]].coord.x;
		s(r.node_variable_y(i)) = _node[free_to_node[i]].coord.y;
	}

	//Preallocating
	Eigen::Vector<real, Eigen::Dynamic>
		z(r.equation_number());							//Should-be-zero value
	Eigen::Vector<real, Eigen::Dynamic>
		m(r.equation_number());							//Modification
	Eigen::Matrix<real, Eigen::Dynamic, Eigen::Dynamic>
		d(r.equation_number(), r.variable_number());	//Derivative of should-be-zero

	//Calculating tolerance
	real tolerance;
	{
		real minforce = std::numeric_limits<real>::infinity();
		for (uint i = 0; i < _force.size(); i++)
		{
			real newforce = sqrt(sqr(_force[i].direction.x) + sqr(_force[i].direction.y));
			if (newforce < minforce) minforce = newforce;
		}
		tolerance = minforce / 1000.0;
	}

	//Iterating
	real last_error = 0.0;
	uint not_converge_count = 0;
	while (true)
	{
		//Calculating should-be-zero and derivative value
		//Setting external forces for free nodes (zeroing should-be-zero)
		for (uint i = 0; i < _force.size(); i++)
		{
			uint node = _force[i].node;
			if (_node[node].free)
			{
				uint free = node_to_free[node];
				z(r.node_variable_x(free)) = _force[i].direction.x;
				z(r.node_variable_y(free)) = _force[i].direction.y;
			}
		}
		//Setting that forces of free _node do not depend from coordinates of free _node (zeroing derivative)
		for (uint i = 0; i < _stick.size(); i++)
		{
			const uint *node = _stick[i].node;
			for (uint j = 0; j < 2; j++)
			{
				if (_node[node[j]].free)
				{
					uint free_j = node_to_free[node[j]];
					d(r.node_equation_fx(free_j), r.node_variable_x(free_j)) = 0.0;
					d(r.node_equation_fx(free_j), r.node_variable_y(free_j)) = 0.0;
					d(r.node_equation_fy(free_j), r.node_variable_x(free_j)) = 0.0;
					d(r.node_equation_fy(free_j), r.node_variable_y(free_j)) = 0.0;
					if (_node[node[j ^ 1]].free)
					{
						uint free_j1 = node_to_free[node[j ^ 1]];
						d(r.node_equation_fx(free_j), r.node_variable_x(free_j1)) = 0.0;
						d(r.node_equation_fx(free_j), r.node_variable_y(free_j1)) = 0.0;
						d(r.node_equation_fy(free_j), r.node_variable_x(free_j1)) = 0.0;
						d(r.node_equation_fy(free_j), r.node_variable_y(free_j1)) = 0.0;
					}
				}
			}
		}

		//Setting forces and derivatives for pairs of _node
		for (uint i = 0; i < _stick.size(); i++)
		{
			//Getting delta between nodes
			Coord delta;
			const uint *node = _stick[i].node;
			{
				Coord coord[2];
				for (uint j = 0; j < 2; j++)
				{
					if (_node[node[j]].free)
					{
						uint free = node_to_free[node[j]];
						coord[j].x = s(r.node_variable_x(free));
						coord[j].y = s(r.node_variable_y(free));
					}
					else
					{
						coord[j] = _node[node[j]].coord;
					}
				}
				delta = coord[1] - coord[0];
			}
			//Setting should-be-zero
			const Material *material = _material[_stick[i].material];
			real length = sqrt(sqr(delta.x) + sqr(delta.y));
			real strain = length / lengths[i] - 1.0;
			real force = _stick[i].area * material->stress(strain);
			for (uint j = 0; j < 2; j++)
			{
				if (_node[node[j]].free)
				{
					uint free = node_to_free[node[j]];
					real sign = j == 0 ? 1.0 : -1.0;
					z(r.node_variable_x(free)) += sign * force * delta.x / length;
					z(r.node_variable_y(free)) += sign * force * delta.y / length;
				}
			}
			//Setting derivative
			real dldx0 = -delta.x / length;
			real dfx0dx0 = _stick[i].area * (
				material->stress(strain) * (-length - dldx0 * delta.x) / sqr(length) +
				material->derivative(strain) * dldx0 * delta.x / (lengths[i] * length)
				);
			real dfy0dx0 = _stick[i].area * (
				material->stress(strain) * (-dldx0 * delta.y) / sqr(length) +
				material->derivative(strain) * dldx0 * delta.y / (lengths[i] * length)
				);
			real dldy0 = -delta.y / length;
			real dfx0dy0 = _stick[i].area * (
				material->stress(strain) * (-dldy0 * delta.x) / sqr(length) +
				material->derivative(strain) * dldy0 * delta.x / (lengths[i] * length)
				);
			real dfy0dy0 = _stick[i].area * (
				material->stress(strain) * (-length - dldy0 * delta.y) / sqr(length) +
				material->derivative(strain) * dldy0 * delta.y / (lengths[i] * length)
				);
			for (uint j = 0; j < 2; j++)
			{
				if (_node[node[j]].free)
				{
					uint free_j = node_to_free[node[j]];
					real sign = j == 0 ? -1.0 : 1.0;
					d(r.node_equation_fx(free_j), r.node_variable_x(free_j)) += sign * dfx0dx0;
					d(r.node_equation_fx(free_j), r.node_variable_y(free_j)) += sign * dfx0dy0;
					d(r.node_equation_fy(free_j), r.node_variable_x(free_j)) += sign * dfy0dx0;
					d(r.node_equation_fy(free_j), r.node_variable_y(free_j)) += sign * dfy0dy0;
					if (_node[node[j ^ 1]].free)
					{
						uint free_j1 = node_to_free[node[j ^ 1]];
						d(r.node_equation_fx(free_j), r.node_variable_x(free_j1)) -= sign * dfx0dx0;
						d(r.node_equation_fx(free_j), r.node_variable_y(free_j1)) -= sign * dfx0dy0;
						d(r.node_equation_fy(free_j), r.node_variable_x(free_j1)) -= sign * dfy0dx0;
						d(r.node_equation_fy(free_j), r.node_variable_y(free_j1)) -= sign * dfy0dy0;
					}
				}
			}
		}
		//Normal qutting
		real error = 0.0;
		for (int i = 0; i < z.rows(); i++)
		{
			if (abs(z(i)) > error) error = abs(z(i));
		}
		if (error < tolerance) break;
		else if (error < last_error) not_converge_count = 0;
		else if (++not_converge_count == 1000) break;//throw (std::runtime_error("Simulation does not converge"));

		//Solving
		Eigen::HouseholderQR<Eigen::Matrix<real, Eigen::Dynamic, Eigen::Dynamic>> qr(d);
		m = qr.solve(z);

		//Error handling & "flowing"
		bool flow = false;
		for (int i = 0; i < s.rows(); i++)
		{
			if (m(i) != m(i) || abs(m(i)) == std::numeric_limits<real>::infinity())
			{
				flow = true; break;
			}
		}

		if (!flow) s -= m;
		else
		{
			//Determining of flow coefficient
			real flow_coef = std::numeric_limits<real>::infinity();
			for (uint i = 0; i < _stick.size(); i++)
			{
				const uint *node = _stick[i].node;
				Coord coord[2];
				for (uint j = 0; j < 2; j++)
				{
					if (_node[node[j]].free)
					{
						uint free = node_to_free[node[j]];
						coord[j].x = s(r.node_variable_x(free));
						coord[j].y = s(r.node_variable_y(free));
					}
					else
					{
						coord[j] = _node[node[j]].coord;
					}
				}
				for (uint j = 0; j < 2; j++)
				{
					if (_node[node[j]].free)
					{
						uint free = node_to_free[node[j]];
						real newcoef = coord[0].distance(coord[1]) /
							sqrt(sqr(z(r.node_equation_fx(free))) + sqr(z(r.node_equation_fy(free))));
						if (newcoef < flow_coef) flow_coef = newcoef;
					}
				}
			}
			s += 0.001 * flow_coef * z;
		}
	}

	//Setting node coordinates from state vector
	for (uint i = 0; i < free_to_node.size(); i++)
	{
		_node[free_to_node[i]].coord_simulated.x = s(r.node_variable_x(i));
		_node[free_to_node[i]].coord_simulated.y = s(r.node_variable_y(i));
	}
}

bool p6::Construction::simulation() const
{
	return _simulation;
}

p6::Construction::~Construction()
{
	for (uint i = 0; i < _material.size(); i++) delete _material[i];
}