#include "config.hpp"
#include <dlib/config_reader.h>
#include <climits>

namespace fp {
using namespace dlib;

const static int SC_WIDTH = 320;
const static int SC_HEIGHT = 240;

Config::SourceConfig::SourceConfig():
    _width(SC_WIDTH),
    _height(SC_HEIGHT)
    {}

const static int SC_FROM = 0;
const static int SC_TOTAL = INT_MAX;

Config::ProcessorConfig::ProcessorConfig():_from(SC_FROM), _total(SC_TOTAL)
{}

const static int SC_MOG2_HISTORY = 500;
const static int SC_MOG2_THRESHOLD = 16;
const static int SC_MOG2_SHADOW = false;
const static double SC_MOG2_LEARNING_RATE = -1.0;

Config::Mog2Config::Mog2Config():
    _learningRate(SC_MOG2_LEARNING_RATE),
    _history(SC_MOG2_HISTORY),
    _threshold(SC_MOG2_THRESHOLD),
    _shadow(SC_MOG2_SHADOW)
{}

    void print_config_reader_contents (
        const config_reader& cr,
        int depth=1
    )
    {
        // Make a string with depth*4 spaces in it.
        const std::string padding(depth*4, ' ');

        // We can obtain a list of all the keys and sub-blocks defined
        // at the current level in the config reader like so:
        std::vector<std::string> keys, blocks;
        cr.get_keys(keys);
        cr.get_blocks(blocks);

        // Now print all the key/value pairs
        for (unsigned long i = 0; i < keys.size(); ++i)
            std::cout << padding << keys[i] << " = " << cr[keys[i]] << std::endl;

        // Now print all the sub-blocks.
        for (unsigned long i = 0; i < blocks.size(); ++i)
        {
            // First print the block name
            std::cout << padding << blocks[i] << " { " << std::endl;
            // Now recursively print the contents of the sub block.  Note that the cr.block()
            // function returns another config_reader that represents the sub-block.
            print_config_reader_contents(cr.block(blocks[i]), depth+1);
            std::cout << padding << "}" << std::endl;
        }
    }

    const Config & Config::parse(const std::string &fileName)
    {
        Config *cfg = new Config();

        std::ifstream fin(fileName.c_str(),std::ios::binary);
        if(fin){
            config_reader cr(fin);
            // print config to the console
            print_config_reader_contents(cr);
            // parse config
            cfg->sourceConfig().setUri(get_option(cr,"VideoSource.uri",""));
            cfg->sourceConfig().setWidth(get_option(cr,"VideoSource.width",SC_WIDTH));
            cfg->sourceConfig().setHeight(get_option(cr,"VideoSource.height",SC_HEIGHT));

            cfg->processorConfig().setFrom(get_option(cr,"ProcessorConfig.from",SC_FROM));
            cfg->processorConfig().setTotal(get_option(cr,"ProcessorConfig.total",SC_TOTAL));

            cfg->mog2Config().setHistory(get_option(cr,"Mog2.history",SC_MOG2_HISTORY));
            cfg->mog2Config().setThreshold(get_option(cr,"Mog2.threshold",SC_MOG2_THRESHOLD));
            cfg->mog2Config().setShadow(get_option(cr,"Mog2.shadow",SC_MOG2_SHADOW));
            cfg->mog2Config().setLearningRate(get_option(cr,"Mog2.learning_rate",SC_MOG2_LEARNING_RATE));
        }

    }
}
