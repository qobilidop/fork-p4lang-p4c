/**
 * @author Timotej Ponek, FIT VUT Brno
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "graph_visitor.h"

#include "graphs.h"
#include "lib/nullstream.h"

namespace graphs {

void Graph_visitor::writeGraphToFile(const Graph &g, const std::string &name) {
    auto path = graphsDir / (name + ".dot");
    auto out = openFile(path, false);
    if (out == nullptr) {
        ::error(ErrorType::ERR_IO, "Failed to open file %1%", path);
        return;
    }
    // Custom label writers not supported with subgraphs, so we populate
    // *_attribute_t properties instead using our GraphAttributeSetter class.
    boost::write_graphviz(*out, g);
}

const char *Graph_visitor::getType(const VertexType &v_type) {
    switch (v_type) {
        case VertexType::TABLE:
            return "table";
            break;
        case VertexType::KEY:
            return "key";
            break;
        case VertexType::ACTION:
            return "action";
            break;
        case VertexType::CONDITION:
            return "condition";
            break;
        case VertexType::SWITCH:
            return "switch";
            break;
        case VertexType::STATEMENTS:
            return "statements";
            break;
        case VertexType::CONTROL:
            return "control";
            break;
        case VertexType::STATE:
            return "state";
            break;
        case VertexType::OTHER:
        default:
            return "other";
            break;
    }
}

const char *Graph_visitor::getPrevType(const PrevType &prev_type) {
    switch (prev_type) {
        case PrevType::Control:
            return "control";
        case PrevType::Parser:
            return "parser";
        default:
            return "err";
            break;
    }
}

void Graph_visitor::forLoopJson(std::vector<Graph *> &graphsArray, PrevType node_type) {
    for (auto g : graphsArray) {
        auto *block = new Util::JsonObject();
        programBlocks->emplace_back(block);

        block->emplace("type"_cs, getPrevType(node_type));
        block->emplace("name"_cs, boost::get_property(*g, boost::graph_name));

        auto *nodesArray = new Util::JsonArray();
        block->emplace("nodes"_cs, nodesArray);

        auto *parserEdges = new Util::JsonArray();
        block->emplace("transitions"_cs, parserEdges);

        auto subg = *g;

        auto vertices = boost::vertices(subg);
        for (auto &vit = vertices.first; vit != vertices.second; ++vit) {
            auto node = new Util::JsonObject();
            nodesArray->emplace_back(node);
            node->emplace("node_nmb"_cs, *vit);

            const auto &vinfo = subg[*vit];

            node->emplace("name"_cs, vinfo.name.escapeJson());
            node->emplace("type"_cs, getType(vinfo.type));
            node->emplace("type_enum"_cs, (unsigned)vinfo.type);
        }

        auto edges = boost::edges(subg);
        for (auto &eit = edges.first; eit != edges.second; ++eit) {
            auto edge = new Util::JsonObject();
            parserEdges->emplace_back(edge);

            // answer https://stackoverflow.com/a/12001149
            auto from = boost::source(*eit, subg);
            auto to = boost::target(*eit, subg);

            edge->emplace("from"_cs, from);
            edge->emplace("to"_cs, to);

            edge->emplace("cond"_cs, boost::get(boost::edge_name, subg, *eit).escapeJson());
        }
    }
}

void Graph_visitor::forLoopFullGraph(std::vector<Graph *> &graphsArray, fullGraphOpts *opts,
                                     PrevType prev_type) {
    unsigned long t_prev_adder = opts->node_i;

    for (auto g_ : graphsArray) {
        auto &subfg = opts->fg.create_subgraph();
        g = &subfg;

        // set subg properties
        boost::get_property(subfg, boost::graph_name) =
            "cluster" + std::to_string(opts->cluster_i++);
        boost::get_property(subfg, boost::graph_graph_attribute)["label"_cs] =
            boost::get_property(*g_, boost::graph_name);
        boost::get_property(subfg, boost::graph_graph_attribute)["style"_cs] = "bold"_cs;
        boost::get_property(subfg, boost::graph_graph_attribute)["fontsize"_cs] = "22pt"_cs;

        // No statements in graph, merge "__START__" and "__EXIT__" nodes
        if (g_->m_global_vertex.size() == 2) {
            add_vertex(cstring("Empty body"), VertexType::EMPTY);
        } else {
            boost::copy_graph(*g_, subfg, boost::edge_copy(edge_name_copier(*g_, subfg)));
        }

        // Connect subgraphs
        if (opts->cluster_i > 1) {
            if (prev_type == PrevType::Parser) {
                add_edge(opts->node_i - 2, opts->node_i, cstring::empty, opts->cluster_i);
                prev_type = PrevType::Control;
            } else {
                add_edge(t_prev_adder, opts->node_i, cstring::empty, opts->cluster_i);
            }
        }

        // If "__START__" and "__EXIT__" nodes merged, increase opts->node_i only by one
        if (g_->m_global_vertex.size() == 2) {
            t_prev_adder = opts->node_i;
            opts->node_i += 1;
        } else {
            t_prev_adder = opts->node_i + 1;
            opts->node_i += g_->m_global_vertex.size();
        }
    }
}

void Graph_visitor::process(std::vector<Graph *> &controlGraphsArray,
                            std::vector<Graph *> &parserGraphsArray) {
    if (graphs) {
        for (auto g : controlGraphsArray) {
            GraphAttributeSetter()(*g);
            writeGraphToFile(*g, boost::get_property(*g, boost::graph_name));
        }
        for (auto g : parserGraphsArray) {
            GraphAttributeSetter()(*g);
            writeGraphToFile(*g, boost::get_property(*g, boost::graph_name));
        }
    }

    if (fullGraph) {
        fullGraphOpts opts;

        boost::get_property(opts.fg, boost::graph_name) = "fullGraph";
        // Enables edges with tails between clusters.
        boost::get_property(opts.fg, boost::graph_graph_attribute)["compound"_cs] = "true"_cs;

        forLoopFullGraph(parserGraphsArray, &opts, PrevType::Parser);
        forLoopFullGraph(controlGraphsArray, &opts, PrevType::Parser);

        GraphAttributeSetter()(opts.fg);
        writeGraphToFile(opts.fg, "fullGraph");
    }

    if (jsonOut) {
        json = new Util::JsonObject();

        // Remove '.p4' and path from program name.
        json->emplace("name"_cs, filename.stem());
        programBlocks = new Util::JsonArray();
        json->emplace("nodes"_cs, programBlocks);

        forLoopJson(parserGraphsArray, PrevType::Parser);
        forLoopJson(controlGraphsArray, PrevType::Control);

        std::ofstream file(graphsDir / "fullGraph.json");
        file << json->toString() << std::endl;
        file.close();
    }
}

}  // namespace graphs
