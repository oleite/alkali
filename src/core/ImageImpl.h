#pragma once
#include <map>
#include <ranges>

#include "Rect.h"

struct LayerInfo
{
    LayerName name;
    std::vector<ChannelName> channelNames;
};

struct Image::Impl
{
    int width;
    int height;
    std::vector<LayerInfo> layers;
    Rect dataBounds;

    LayerInfo *layerInfo(const LayerName &name)
    {
        auto it = std::ranges::find(
            layers,
            name,
            &LayerInfo::name);

        if (it == layers.end())
            return nullptr;

        return &*it;
    }
    std::vector<LayerName> layerNames() const
    {
        std::vector<LayerName> names;
        names.reserve(layers.size());
        for (const auto &layer : layers)
        {
            names.push_back(layer.name);
        }
        return names;
    }
    const std::vector<ChannelName> &channelNames(const LayerName &name) const
    {
        static const std::vector<ChannelName> empty;

        const auto it = std::ranges::find(layers, name, &LayerInfo::name);
        if (it == layers.end())
            return empty;

        return it->channelNames;
    }
};