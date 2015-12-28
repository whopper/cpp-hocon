#include <hocon/config_value.hpp>
#include <internal/container.hpp>
#include <internal/config_util.hpp>
#include <hocon/config_object.hpp>
#include <internal/objects/simple_config_object.hpp>
#include <internal/simple_config_origin.hpp>
#include <internal/config_exception.hpp>

using namespace std;

namespace hocon {

    config_value::config_value(shared_origin origin) :
        _origin(move(origin)) { }

    string config_value::transform_to_string() const {
        return "";
    }

    shared_origin const& config_value::origin() const {
        return _origin;
    }

    resolve_status config_value::get_resolve_status() const {
        return resolve_status::RESOLVED;
    }

    resolve_status config_value::resolve_status_from_values(std::vector<shared_value> values) {
        for (auto v : values) {
            if (v->get_resolve_status() == resolve_status::UNRESOLVED) {
                return resolve_status::UNRESOLVED;
            }
        }
        return resolve_status::RESOLVED;
    }

    resolve_status config_value::resolve_status_from_boolean(bool resolved) {
        return resolved ? resolve_status::RESOLVED : resolve_status::UNRESOLVED;
    }

    string config_value::render() const {
        return render(config_render_options());
    }

    string config_value::render(config_render_options options) const {
        string result;
        render(result, 0, true, "", options);
        return result;
    }

    void config_value::render(std::string &result, int indent, bool at_root, std::string at_key,
                                         config_render_options options) const {
        if (!at_key.empty()) {
            string rendered_key;
            if (options.get_json()) {
                rendered_key = render_json_string(at_key);
            } else {
                rendered_key = render_string_unquoted_if_possible(at_key);
            }

            result += rendered_key;
            if (options.get_json()) {
                result += options.get_formatted() ? " : " : ":";
            } else {
                // in non-JSON we can omit the color or equals before an object
                if (dynamic_cast<const config_object*>(this)) {
                    if (options.get_formatted()) {
                        result += " ";
                    }
                } else {
                    result += "=";
                }
            }
        }
        render(result, indent, at_root, options);
    }

    void config_value::render(std::string &result, int indent, bool at_root,
                                       config_render_options options) const {
        result += transform_to_string();
    }

    shared_config config_value::at_path(shared_origin origin, path raw_path) const {
        path parent = raw_path.parent();
        shared_config result = at_key(origin, *raw_path.last());
        while (!parent.empty()) {
            string key = *parent.last();
            result = result->at_key(origin, key);
            parent = parent.parent();
        }
        return result;
    }

    shared_config config_value::at_key(shared_origin origin, std::string const& key) const {
        unordered_map<string, shared_value> map { make_pair(key, shared_from_this()) };
        return simple_config_object(origin, map).to_config();
    }

    shared_config config_value::at_key(std::string const& key) const {
        return at_key(make_shared<simple_config_origin>("at_key(" + key + ")"), key);
    }

    shared_config config_value::at_path(std::string const& path_expression) const {
        shared_origin origin = make_shared<simple_config_origin>("at_path(" + path_expression + ")");
        return at_path(move(origin), path::new_path(path_expression));
    }

    vector<shared_value> config_value::replace_child_in_list(std::vector<shared_value> list,
                                               shared_value child,
                                               shared_value replacement) {

        auto it = find_if(list.begin(), list.end(), [&](const shared_value &value) { return value == child; });

        if (it == list.end()) {
            throw config_exception("tried to replace " + child->render() +
                                   " which is not in "); // TODO: how should we print child and list?
        }

        vector<shared_value> new_stack{list};

        if (replacement) {
            new_stack.insert(it, replacement);
        } else {
            new_stack.erase(it);
        }

        return new_stack;  // TODO: original implementation returns null if empty. Should we do that here or just return an empty vector?
    }

    bool config_value::has_descendant_in_list(std::vector<shared_value> list, shared_value descendant) {

        auto it = find_if(list.begin(), list.end(), [&](const shared_value &value) { return value == descendant; });

        if (it != list.end()) {
            return true;
        }

        // now the expensive traversal
        for (shared_value v : list) {
            if (dynamic_pointer_cast<const container>(v)->has_descendant(descendant)) {
                return true;
            }
        }

        return false;
    }

}  // namespace hocon
