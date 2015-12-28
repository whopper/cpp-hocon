#include <internal/simple_config_list.hpp>
#include <internal/config_exception.hpp>

using namespace std;

namespace hocon {

    simple_config_list::simple_config_list(shared_origin origin, std::vector<shared_value> value) :
        simple_config_list(origin, value, config_value::resolve_status_from_values(value)) { }


    simple_config_list::simple_config_list(shared_origin origin, std::vector<shared_value> value, resolve_status status) :
            config_value(move(origin)), _value(move(value)), _resolved(status == resolve_status::RESOLVED) {

        // kind of an expensive debug check (makes this constructor pointless)
        if (status != config_value::resolve_status_from_values(value)) {
            throw config_exception("simple_config_list created with wrong resolve status: "); // TODO: how to append `this` ?
        }
    }

    config_value_type simple_config_list::value_type() const {
        return config_value_type::LIST;
    }

    resolve_status simple_config_list::get_resolve_status() const {
        return config_value::resolve_status_from_boolean(_resolved);
    }

    shared_ptr<simple_config_list> simple_config_list::replace_child(shared_value child, shared_value replacement) {
        vector<shared_value> new_list = replace_child_in_list(_value, child, replacement);

        if (new_list.empty()) { // TODO: if replace_child_in_list ends up returning null instead of empty, chnage this
            return nullptr;
        } else {
            // we use the constructor flavor that will recompute the resolve
            // status
            return make_shared<simple_config_list>(origin(), new_list);
        }
    }

    bool simple_config_list::has_descendant(shared_value descendant) {
        return has_descendant_in_list(_value, descendant);
    }

    shared_ptr<simple_config_list> simple_config_list::modify(no_exceptions_modifier modifier,
                                                              resolve_status new_resolve_status) {
        try {
            return modify_may_throw(make_shared<no_exceptions_modifier>(modifier), new_resolve_status);
        } catch (config_exception& ex) {
            throw new config_exception("Unexpected checked exception"); // TODO: how to actually throw this. Orig uses BugOrBroken and passes ex
        }
    }

    shared_ptr<simple_config_list> simple_config_list::modify_may_throw(shared_ptr<modifier> modifier, resolve_status new_resolve_status) {
        // lazy-create for optimization
        vector<shared_value> changed;
        int i = 0;

        for (auto v : _value) {
            auto modified = modifier->modify_child_may_throw("" /* key */, v);

            if (changed.empty() && modified != v) {
                for (int j = 0; j < i; ++j) {
                    changed.push_back(_value[j]);
                }
            }

            // Once the new list is created, all elements
            // have to go in it. If modify_child returned
            // null, we drop that element.
            if (!changed.empty() && modified != nullptr) {
                changed.push_back(modified);
            }

            i += 1;
        }

        if (!changed.empty()) {
        // TODO: how to check if new_resolve_status was provided as param?
/*
            if (new_resolve_status) {
                return make_shared<simple_config_list>(origin(), changed, new_resolve_status);
            } else {
                return make_shared<simple_config_list>(origin(), changed);
            }
*/
            return make_shared<simple_config_list>(origin(), changed, new_resolve_status);
        } else {
            return dynamic_pointer_cast<simple_config_list>(shared_from_this());   // TODO: there's no way this is right
        }
    }
}  // namespace hocon
