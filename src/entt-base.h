/* Common reusable entt components */

#pragma once

struct Parent
{
  entt::entity parent;
};

struct Children
{
  std::vector< entt::entity > children;
};



