#include <hocon/config_value.hpp>
#include <internal/config_util.hpp>
#include <hocon/config_object.hpp>
#include <internal/objects/simple_config_object.hpp>
#include <internal/simple_config_origin.hpp>
#include <internal/config_delayed_merge.hpp>

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

    void config_value::require_not_ignoring_fallbacks() const {
        if (ignores_fallbacks()) {
            throw config_exception("method should not have been called with ignores_fallbacks=true"); // TODO: include class name here?
        }
    }

    bool config_value::ignores_fallbacks() const {
        return get_resolve_status() == resolve_status::RESOLVED;
    }

    shared_value config_value::with_fallbacks_ignored() const {
        if (ignores_fallbacks()) {
            return shared_from_this();
        } else {
            throw config_exception("value class doesn't implement forced fallback-ignoring"); // TODO: append `this?
        }
    }

    shared_value config_value::construct_delayed_merge(shared_origin origin, std::vector<shared_value> stack) const {
        return make_shared<config_delayed_merge>(move(origin), move(stack));
    }

    shared_value config_value::merged_with_the_unmergeable(std::vector<shared_value> stack,
                                                           std::shared_ptr<unmergeable> fallback) const {

        require_not_ignoring_fallbacks();

        // if we turn out to be an object, and the fallback also does,
        // then a merge may be required; delay until we resolve.
        std::vector<shared_value> new_stack { stack };

        auto unmerged_values = fallback->unmerged_values();
        new_stack.insert(new_stack.end(), unmerged_values.begin(), unmerged_values.end());
        return construct_delayed_merge(config_object::merge_origins(new_stack), new_stack);
    }

    shared_value config_value::merged_with_the_unmergeable(std::shared_ptr<unmergeable> fallback) const {
        require_not_ignoring_fallbacks();

        return merged_with_the_unmergeable({shared_from_this()}, move(fallback));
    }

    shared_value config_value::delay_merge(std::vector<shared_value> stack, shared_value fallback) const {
        // if we turn out to be an object, and the fallback also does,
        // then a merge may be required.
        // if we contain a substitution, resolving it may need to look
        // back to the fallback.
        std::vector<shared_value> new_stack { stack };
        new_stack.push_back(fallback);
        return construct_delayed_merge(config_object::merge_origins(new_stack), new_stack);
    }

    shared_value config_value::merged_with_object(vector<shared_value> stack, shared_object fallback) const {
        require_not_ignoring_fallbacks();

        if (dynamic_pointer_cast<const shared_object>(shared_from_this())) {
            throw config_exception("Objects must reimplement merged_with_object");
        }

        return merged_with_non_object(move(stack), move(fallback));
    }

    shared_value config_value::merged_with_non_object(vector<shared_value> stack, shared_value fallback) const {
        require_not_ignoring_fallbacks();

        if (get_resolve_status() == resolve_status::RESOLVED) {
            // falling back to a non-object doesn't merge anything, and also
            // prohibits merging any objects that we fall back to later.
            // so we have to switch to ignoresFallbacks mode.
            return with_fallbacks_ignored();
        } else {
            // if unresolved, we may have to look back to fallbacks as part of
            // the resolution process, so always delay
            return delay_merge(move(stack), move(fallback));
        }
    }

    shared_value config_value::merged_with_non_object(shared_value fallback) const {
        require_not_ignoring_fallbacks();

        return merged_with_non_object({shared_from_this()}, move(fallback));
    }

    shared_value config_value::merged_with_object(shared_object fallback) const {
        require_not_ignoring_fallbacks();

        return merged_with_object({shared_from_this()}, move(fallback));
    }

}  // namespace hocon
