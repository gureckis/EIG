#include "nodes.h"

#define evaluate_child(child_num) \
    for (int _i = 0; _i < (child_num); ++ _i) \
        this -> params[_i] -> evaluate(h, lambda_var)

void EqualNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    this -> _val.b = (this -> params[0] -> val().i == this -> params[1] -> val().i);
}

void SetEqualNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    this -> _val.b = true;
    int first_v = *(((SetFuncNode*)this -> params[0]) -> set.begin());
    for (auto i: ((SetFuncNode*)this -> params[0]) -> set) {
        if (i != first_v) {
            this -> _val.b = false;
            break;
        }
    }
}

void GreaterNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    this -> _val.b = (this -> params[0] -> val().i > this -> params[1] -> val().i);
}

void LessNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    this -> _val.b = (this -> params[0] -> val().i < this -> params[1] -> val().i);
}

void PlusNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    this -> _val.i = this -> params[0] -> val().i + this -> params[1] -> val().i;
}

void MinusNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    this -> _val.i = this -> params[0] -> val().i - this -> params[1] -> val().i;
}

void SumNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    this -> _val.i = 0;
    for (auto i: ((SetFuncNode*)this -> params[0]) -> set) {
        this -> _val.i += i;
    }
}

void AndNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    this -> _val.b = this -> params[0] -> val().b && this -> params[1] -> val().b;
}

void OrNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    this -> _val.b = this -> params[0] -> val().b || this -> params[1] -> val().b;
}

void NotNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    this -> _val.b = !(this -> params[0] -> val().b);
}

void RowNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    this -> _val.i = int(this -> params[0] -> val().p[0]);
}

void ColNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    this -> _val.i = int(this -> params[0] -> val().p[1]);
}

void AnyNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    this -> _val.b = false;
    for (auto i: ((SetFuncNode*)this -> params[0]) -> set) {
        if (i == 1) {
            this -> _val.b = true;
            break;
        }
    }
}

void AllNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    this -> _val.b = true;
    for (auto i: ((SetFuncNode*)this -> params[0]) -> set) {
        if (i == 0) {
            this -> _val.b = false;
            break;
        }
    }
}

void TopLeftNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    short x = h -> h, y = h -> w;
    for (auto i: ((SetFuncNode*)this -> params[0]) -> set) {
        value_t pos = {i};
        if (pos.p[0] < x || (pos.p[0] == x && pos.p[1] < y))
            x = pos.p[0], y = pos.p[1];
    }
    this -> _val.p[0] = x;
    this -> _val.p[1] = y;
}

void BottomRightNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    short x = 0, y = 0;
    for (auto i: ((SetFuncNode*)this -> params[0]) -> set) {
        value_t pos = {i};
        if (pos.p[0] > x || (pos.p[0] == x && pos.p[1] > y))
            x = pos.p[0], y = pos.p[1];
    }
    this -> _val.p[0] = x;
    this -> _val.p[1] = y;
}

void SetSizeNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    this -> _val.i = ((SetFuncNode*)this -> params[0]) -> set.size();
}

void IsSubsetNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    SetFuncNode* set1 = (SetFuncNode*)this -> params[0];
    SetFuncNode* set2 = (SetFuncNode*)this -> params[1];
    this -> _val.b = true;
    for (auto i: set1 -> set) {
        if (set2 -> set.find(i) == set2 -> set.end()) {
            this -> _val.b = false;
            break;
        }
    }
}

void ColorFuncNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    value_t loc = this -> params[0] -> val();
    int x = loc.p[0], y = loc.p[1];
    this -> _val.i = h -> board[x * h -> w + y];
}

void OrientFuncNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    int ship_label = this -> params[0] -> val().i;
    for (int i = 0; i < h -> ship_cnt; ++ i) {
        if (h -> ships[i].label == ship_label) {
            this -> _val.i = h -> ships[i].orientation;
            break;
        }
    }
    // TODO: Invoke runtime error if ship_label is not in h -> ships
}

// binary trick here
// ORIENTATION_VERTICAL + 1 = 1 => (01)_2
// ORIENTATION_HORIZONTAL + 1 = 2 => (10)_2
#define step_x(orientation) ((orientation + 1) & 1)
#define step_y(orientation) (((orientation + 1) & 2) >> 1)

void TouchFuncNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    int ship_label_1 = this -> params[0] -> val().i;
    int ship_label_2 = this -> params[1] -> val().i;
    int step_x1, step_x2, step_y1, step_y2;
    int size1, size2, x1, y1, x2, y2;
    for (int i = 0; i < h -> ship_cnt; ++ i) {
        if (h -> ships[i].label == ship_label_1) {
            int orientation = h -> ships[i].orientation;
            step_x1 = step_x(orientation);
            step_y1 = step_y(orientation);
            x1 = h -> ships[i].x, y1 = h -> ships[i].y;
            size1 = h -> ships[i].size;
        }
        if (h -> ships[i].label == ship_label_2) {
            int orientation = h -> ships[i].orientation;
            step_x2 = step_x(orientation);
            step_y2 = step_y(orientation);
            x2 = h -> ships[i].x, y2 = h -> ships[i].y;
            size2 = h -> ships[i].size;
        }
    }
    this -> _val.b = false;
    for (int i = 0; i < size1; ++ i) {
        for (int j = 0; j < size2; ++ j) {
            int diff = 0;
            if (x1 > x2) diff += (x1 - x2);
            else diff += (x2 - x1);
            if (y1 > y2) diff += (y1 - y2);
            else diff += (y2 - y1);
            if (diff == 1) {
                this -> _val.b = true;
                return;
            }
            x2 += step_x2; y2 += step_y2;
        }
        x1 += step_x1; y1 += step_y1;
    }
}

void SizeFuncNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    int ship_label = this -> params[0] -> val().i;
    for (int i = 0; i < h -> ship_cnt; ++ i) {
        if (h -> ships[i].label == ship_label) {
            this -> _val.i = h -> ships[i].size;
            break;
        }
    }
}

void ColoredTilesFuncNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    this -> set.clear();
    int ship_label = this -> params[0] -> val().i;
    for (int i = 0; i < h -> ship_cnt; ++ i) {
        if (h -> ships[i].label == ship_label) {
            int orientation = h -> ships[i].orientation;
            int stepx = step_x(orientation);
            int stepy = step_y(orientation);
            int x = h -> ships[i].x, y = h -> ships[i].y;
            for (int j = 0; j < h -> ships[i].size; ++ j) {
                value_t pos;
                pos.p[0] = x, pos.p[1] = y;
                this -> set.insert(pos.i);
                x += stepx, y += stepy;
            }
        }
    }
}

void MapNode::evaluate(Hypothesis* h, int lambda_var) {
    this -> set.clear();
    this -> params[1] -> evaluate(h);
    for (auto i: ((SetFuncNode*)this -> params[1]) -> set) {
        this -> params[0] -> evaluate(h, i);
        this -> set.insert(this -> params[0] -> val().i);
    }
}

void SetNode::evaluate(Hypothesis* h, int lambda_var) {
    this -> set.clear();
    for (auto node_ptr: params) {
        node_ptr -> evaluate(h);
        this -> set.insert(node_ptr -> val().i);
    }
}

// TODO: the following functions are ambiguous when encountering
// repeated elements. Clarification needed.
void SetDiffNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    auto& param_set1 = ((SetFuncNode*)this -> params[0]) -> set;
    auto& param_set2 = ((SetFuncNode*)this -> params[1]) -> set;
    this -> set.clear();
    for (auto i: param_set1) {
        auto it = param_set2.find(i);
        if (it != param_set2.end()) {
            param_set2.erase(it);
        }
        else this -> set.insert(i);
    }
}

void UnionNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    this -> set.clear();
    for (auto i: ((SetFuncNode*)this -> params[0]) -> set)
        this -> set.insert(i);
    for (auto i: ((SetFuncNode*)this -> params[1]) -> set)
        this -> set.insert(i);
}

void IntersectNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(2);
    auto& param_set1 = ((SetFuncNode*)this -> params[0]) -> set;
    auto& param_set2 = ((SetFuncNode*)this -> params[1]) -> set;
    this -> set.clear();
    for (auto i: param_set1) {
        auto it = param_set2.find(i);
        if (it != param_set2.end()) {
            this -> set.insert(i);
            param_set2.erase(it);
        }
    }
}

void UniqueNode::evaluate(Hypothesis* h, int lambda_var) {
    evaluate_child(1);
    this -> set.clear();
    auto& param_set = ((SetFuncNode*)this -> params[0]) -> set;
    for (auto i : param_set)
        // TODO: faster way to do this?
        if (this -> set.find(i) == set.end())
            this -> set.insert(i);
}

void LambdaNode::evaluate(Hypothesis* h, int lambda_var) {
    this -> params[1] -> evaluate(h, lambda_var);
    this -> _val.i = this -> params[1] -> val().i;
}

Node* build_node(std::string node_name) {
    if (node_name == "equal") return new EqualNode();
    else if (node_name == "set_equal") return new SetEqualNode();
    else if (node_name == "greater") return new GreaterNode();
    else if (node_name == "less") return new LessNode();
    else if (node_name == "plus") return new PlusNode();
    else if (node_name == "minus") return new MinusNode();
    else if (node_name == "sum") return new SumNode();
    else if (node_name == "and") return new AndNode();
    else if (node_name == "or") return new OrNode();
    else if (node_name == "not") return new NotNode();
    else if (node_name == "row") return new RowNode();
    else if (node_name == "col") return new ColNode();
    else if (node_name == "any") return new AnyNode();
    else if (node_name == "all") return new AllNode();
    else if (node_name == "topleft") return new TopLeftNode();
    else if (node_name == "bottomright") return new BottomRightNode();
    else if (node_name == "set_size") return new SetSizeNode();
    else if (node_name == "is_subset") return new IsSubsetNode();

    else if (node_name == "color_fn") return new ColorFuncNode();
    else if (node_name == "orient_fn") return new OrientFuncNode();
    else if (node_name == "touch_fn") return new TouchFuncNode();
    else if (node_name == "size_fn") return new SizeFuncNode();
    else if (node_name == "colored_tiles_fn") return new ColoredTilesFuncNode();

    else if (node_name == "map") return new MapNode();
    else if (node_name == "set") return new SetNode();
    else if (node_name == "set_diff") return new SetDiffNode();
    else if (node_name == "union") return new UnionNode();
    else if (node_name == "intersect") return new IntersectNode();
    else if (node_name == "unique") return new UniqueNode();
    else if (node_name == "lambda") return new LambdaNode();

    else if (node_name == "number") return new IntNode();
    else if (node_name == "boolean") return new BoolNode();
    else if (node_name == "color") return new IntNode();
    else if (node_name == "location") return new LocationNode();
    else if (node_name == "orientation") return new IntNode();
    else if (node_name == "lambda_x" or node_name == "lambda_y") return new LambdaVarNode();
    // TODO: invoke runtime error if reach here
}