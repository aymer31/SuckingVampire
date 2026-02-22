#pragma once

#include "core.hpp"
#include "components.hpp"
#include <vector>
#include <memory>
#include <type_traits>
#include <utility>



class UISpawnSystem : public ecs::System
{
public:
    template<typename Comp>
    void spawn_shape_as(ecs::Entity entity, std::unique_ptr<sf::Shape> shape)
    {
        static_assert(
            std::is_assignable_v<decltype(std::declval<Comp>().shape), std::unique_ptr<sf::Shape>>,
            "Comp::shape must be assignable from std::unique_ptr<sf::Shape> (adjust component type)"
        );

        auto& comp = ecs::get_component<Comp>(entity);
        comp.shape = std::move(shape);
    }
};
