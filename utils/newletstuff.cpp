// Close let binding scope
if ((*(expr.nest))[0].type == KEYW && (*(expr.nest))[0].keyw == LET) {
    uvar_maps.pop_back();
}

if (expr.keyw == LET) {
    // Add new uvar map for this new scope
    uvar_maps.push_back(std::unordered_map<std::string, uint64_t>());
    uvar_cnt = 0;
}

for (int i = uvar_maps.size()-1; i >= 0; --i) {
    std::cerr << "Searching map #" << i << "\n";
    auto it = uvar_maps[i].find(word);
    if (it != uvar_maps[i].end()) {
        expr.uvar_num = it->second;
        expr.uvar_dep = i;
        return(expr);
    }
}
std::cerr << "New var # " << uvar_cnt << "to map #" << uvar_maps.size()-1 << "\n";
uvar_maps.back()[word] = uvar_cnt;
expr.uvar_num = uvar_cnt;
expr.uvar_dep = uvar_maps.size()-1;

