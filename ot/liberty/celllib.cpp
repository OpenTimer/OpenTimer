// -----------------------------------------------------------------------------
// Copyright (c) 2024 Partcl, Inc. All rights reserved.
//
// This software is proprietary and confidential. Unauthorized copying, sharing,
// modification, or distribution of this file, via any medium, is strictly prohibited
// without prior written permission from Partcl, Inc.
//
// For inquiries regarding licensing, please contact: contact@partcleda.com
// -----------------------------------------------------------------------------

#include <ot/liberty/celllib.hpp>

namespace ot
{

  // Procedure: _uncomment
  void Celllib::_uncomment(std::vector<char> &buffer)
  {

    auto fsize = buffer.size() > 0 ? buffer.size() - 1 : 0;

    // Mart out the comment
    for (size_t i = 0; i < fsize; ++i)
    {

      // Block comment
      if (buffer[i] == '/' && buffer[i + 1] == '*')
      {
        buffer[i] = buffer[i + 1] = ' ';
        for (i = i + 2; i < fsize; buffer[i++] = ' ')
        {
          if (buffer[i] == '*' && buffer[i + 1] == '/')
          {
            buffer[i] = buffer[i + 1] = ' ';
            i = i + 1;
            break;
          }
        }
      }

      // Line comment
      if (buffer[i] == '/' && buffer[i + 1] == '/')
      {
        buffer[i] = buffer[i + 1] = ' ';
        for (i = i + 2; i < fsize; ++i)
        {
          if (buffer[i] == '\n' || buffer[i] == '\r')
          {
            break;
          }
          else
            buffer[i] = ' ';
        }
      }

      // Pond comment
      if (buffer[i] == '#')
      {
        buffer[i] = ' ';
        for (i = i + 1; i < fsize; ++i)
        {
          if (buffer[i] == '\n' || buffer[i] == '\r')
          {
            break;
          }
          else
            buffer[i] = ' ';
        }
      }
    }
  }

  // Procedure: _tokenize
  void Celllib::_tokenize(const std::vector<char> &buf, std::vector<std::string> &tokens)
  {
    static const std::string dels = "(),:/#[]{}*\"\\";
    const char *beg = buf.data();
    const char *end = buf.data() + buf.size();
    tokens.clear();

    const char *token = nullptr;
    size_t len = 0;

    for (const char *itr = beg; itr != end && *itr != 0; ++itr)
    {
      bool is_del = (dels.find(*itr) != std::string::npos);

      if (std::isspace(*itr) || is_del)
      {
        if (len > 0)
        {
          tokens.emplace_back(token, len); // Use std::string to copy data safely
          token = nullptr;
          len = 0;
        }

        if (*itr == '(' || *itr == ')' || *itr == '{' || *itr == '}')
        {
          tokens.emplace_back(1, *itr);
        }
      }
      else
      {
        if (len == 0)
          token = itr;
        ++len;
      }
    }

    if (len > 0)
    {
      tokens.emplace_back(token, len);
    }
  }

  // ------------------------------------------------------------------------------------------------

  // Function: to_string
  std::string to_string(DelayModel m)
  {
    switch (m)
    {
    case DelayModel::GENERIC_CMOS:
      return "generic_cmos";
      break;

    case DelayModel::TABLE_LOOKUP:
      return "table_lookup";
      break;

    case DelayModel::CMOS2:
      return "cmos2";
      break;

    case DelayModel::PIECEWISE_CMOS:
      return "piecewise_cmos";
      break;

    case DelayModel::DCM:
      return "dcm";
      break;

    case DelayModel::POLYNOMIAL:
      return "polynomial";
      break;

    default:
      return "undefined";
      break;
    }
  }

  // Function: lut_template
  const LutTemplate *Celllib::lut_template(const std::string &name) const
  {
    if (auto itr = lut_templates.find(name); itr == lut_templates.end())
    {
      return nullptr;
    }
    else
    {
      return &(itr->second);
    }
  }

  // Function: lut_template
  LutTemplate *Celllib::lut_template(const std::string &name)
  {
    if (auto itr = lut_templates.find(name); itr == lut_templates.end())
    {
      return nullptr;
    }
    else
    {
      return &(itr->second);
    }
  }

  // Function: cell
  const Cell *Celllib::cell(const std::string &name) const
  {
    if (auto itr = cells.find(name); itr == cells.end())
    {
      return nullptr;
    }
    else
    {
      return &(itr->second);
    }
  }

  // Function: cell
  Cell *Celllib::cell(const std::string &name)
  {
    if (auto itr = cells.find(name); itr == cells.end())
    {
      return nullptr;
    }
    else
    {
      return &(itr->second);
    }
  }

  // Function: _extract_operating_conditions
  std::optional<float> Celllib::_extract_operating_conditions(token_iterator &itr, const token_iterator end)
  {

    std::optional<float> voltage;
    std::string operating_condition_name;

    if (itr = on_next_parentheses(
            itr,
            end,
            [&](auto &name) mutable
            { operating_condition_name = name; });
        itr == end)
    {
      OT_LOGF("can't find lut template name");
    }

    // Extract the lut template group
    if (itr = std::find(itr, end, "{"); itr == end)
    {
      OT_LOGF("can't find lut template group brace '{'");
    }

    // std::cout << lt.name << std::endl;

    int stack = 1;

    while (stack && ++itr != end)
    {

      // variable 1
      if (*itr == "voltage")
      { // Read the variable.

        if (++itr == end)
        {
          OT_LOGF("volate error in operating_conditions template ", operating_condition_name);
        }

        voltage = std::strtof(std::string(*itr).c_str(), nullptr);
      }
      else if (*itr == "}")
      {
        stack--;
      }
      else if (*itr == "{")
      {
        stack++;
      }
      else
      {
      }
    }

    if (stack != 0 || *itr != "}")
    {
      OT_LOGF("can't find operating_conditions template group brace '}'");
    }

    return voltage;
  }

  // Function: _extract_sequential_info
  SequentialInfo Celllib::_extract_sequential_info(token_iterator &itr, const token_iterator end)
  {

    SequentialInfo info;

    if (itr = on_next_parentheses(
            itr,
            end,
            [&](auto &name) mutable
            { info.clocked_on = name; });
        itr == end)
    {
      OT_LOGF("can't find sequential info");
    }

    // Extract the lut template group
    if (itr = std::find(itr, end, "{"); itr == end)
    {
      OT_LOGF("can't find sequential info group brace '{'");
    }

    // std::cout << lt.name << std::endl;

    int stack = 1;

    while (stack && ++itr != end)
    {
      if (*itr == "clocked_on")
      {
        if (++itr == end)
        {
          OT_LOGF("clocked_on error in sequential info ", info.clocked_on);
        }

        std::string value;
        while (itr != end && *itr != ";")
        {
          if (!value.empty())
          {
            value += " ";
          }
          value += *itr++;
        }
        info.clocked_on = value;
      }
      else if (*itr == "next_state")
      {
        if (++itr == end)
        {
          OT_LOGF("next_state error in sequential info ", info.clocked_on);
        }

        std::string value;
        while (itr != end && *itr != ";")
        {
          if (!value.empty())
          {
            value += " ";
          }
          value += *itr++;
        }
        info.next_state = value;
      }
      else if (*itr == "data_in")
      {
        if (++itr == end)
        {
          OT_LOGF("data_in error in sequential info ", info.clocked_on);
        }

        info.data_in = *itr;
      }
      else if (*itr == "enable")
      {
        if (++itr == end)
        {
          OT_LOGF("enable error in sequential info ", info.clocked_on);
        }

        info.enable = *itr;
      }
      else if (*itr == "}")
      {
        stack--;
      }
      else if (*itr == "{")
      {
        stack++;
      }
      else
      {
      }
    }

    if (stack != 0 || *itr != "}")
    {
      OT_LOGF("can't find sequential info group brace '}'");
    }

    return info;
  }

  // Function: _extract_lut_template
  LutTemplate Celllib::_extract_lut_template(token_iterator &itr, const token_iterator end)
  {

    LutTemplate lt;

    if (itr = on_next_parentheses(
            itr,
            end,
            [&](auto &name) mutable
            { lt.name = name; });
        itr == end)
    {
      OT_LOGF("can't find lut template name");
    }

    // Extract the lut template group
    if (itr = std::find(itr, end, "{"); itr == end)
    {
      OT_LOGF("can't find lut template group brace '{'");
    }

    // std::cout << lt.name << std::endl;

    int stack = 1;

    while (stack && ++itr != end)
    {

      // variable 1
      if (*itr == "variable_1")
      { // Read the variable.

        if (++itr == end)
        {
          OT_LOGF("variable_1 error in lut template ", lt.name);
        }

        if (auto vitr = lut_vars.find(*itr); vitr != lut_vars.end())
        {
          lt.variable1 = vitr->second;
        }
        else
        {
          OT_LOGW("unexpected lut template variable ", *itr);
        }
      }
      // variable 2
      else if (*itr == "variable_2")
      {

        if (++itr == end)
        {
          OT_LOGF("variable_2 error in lut template ", lt.name);
        }

        if (auto vitr = lut_vars.find(*itr); vitr != lut_vars.end())
        {
          lt.variable2 = vitr->second;
        }
        else
        {
          OT_LOGW("unexpected lut template variable ", *itr);
        }
      }
      // index_1
      else if (*itr == "index_1")
      {
        itr = on_next_parentheses(itr, end, [&](auto &str)
                                  { lt.indices1.push_back(std::strtof(str.data(), nullptr)); });
      }
      // index_2
      else if (*itr == "index_2")
      {
        itr = on_next_parentheses(itr, end, [&](auto &str)
                                  { lt.indices2.push_back(std::strtof(str.data(), nullptr)); });
      }
      else if (*itr == "}")
      {
        stack--;
      }
      else if (*itr == "{")
      {
        stack++;
      }
      else
      {
      }
    }

    if (stack != 0 || *itr != "}")
    {
      OT_LOGF("can't find lut template group brace '}'");
    }

    return lt;
  }

  // Function: _extract_lut
  Lut Celllib::_extract_lut(token_iterator &itr, const token_iterator end)
  {

    Lut lut;

    if (itr = on_next_parentheses(
            itr,
            end,
            [&](auto &name) mutable
            { lut.name = name; });
        itr == end)
    {
      OT_LOGF("can't find lut template name");
    }

    // Set up the template
    lut.lut_template = lut_template(lut.name);

    // Extract the lut group
    if (itr = std::find(itr, end, "{"); itr == end)
    {
      OT_LOGF("group brace '{' error in lut ", lut.name);
    }

    int stack = 1;

    size_t size1 = 1;
    size_t size2 = 1;

    while (stack && ++itr != end)
    {

      if (*itr == "index_1")
      {
        itr = on_next_parentheses(itr, end, [&](auto &v) mutable
                                  { lut.indices1.push_back(std::strtof(v.data(), nullptr)); });

        if (lut.indices1.size() == 0)
        {
          OT_LOGF("syntax error in ", lut.name, " index_1");
        }

        size1 = lut.indices1.size();
      }
      else if (*itr == "index_2")
      {
        itr = on_next_parentheses(itr, end, [&](auto &v) mutable
                                  { lut.indices2.push_back(std::strtof(v.data(), nullptr)); });

        if (lut.indices2.size() == 0)
        {
          OT_LOGF("syntax error in ", lut.name, " index_2");
        }

        size2 = lut.indices2.size();
      }
      else if (*itr == "index_3")
      {
        itr = on_next_parentheses(itr, end, [&](auto &v) mutable
                                  { lut.indices3 = std::vector<float>{std::strtof(v.data(), nullptr)}; });
        if (lut.indices3->size() == 0)
        {
          OT_LOGF("syntax error in ", lut.name, " index_3");
        }
      }
      else if (*itr == "index_4")
      {
        itr = on_next_parentheses(itr, end, [&](auto &v) mutable
                                  { lut.indices4 = std::vector<float>{std::strtof(v.data(), nullptr)}; });
        if (lut.indices4->size() == 0)
        {
          OT_LOGF("syntax error in ", lut.name, " index_4");
        }
      }
      else if (*itr == "values")
      {

        if (lut.indices1.empty())
        {
          if (size1 != 1)
          {
            OT_LOGF("empty indices1 in non-scalar lut ", lut.name);
          }
          lut.indices1.resize(size1);
        }

        if (lut.indices2.empty())
        {
          if (size2 != 1)
          {
            OT_LOGF("empty indices2 in non-scalar lut ", lut.name);
          }
          lut.indices2.resize(size2);
        }

        lut.table.resize(size1 * size2);

        int id{0};
        itr = on_next_parentheses(itr, end, [&](auto &v) mutable
                                  { lut.table[id++] = std::strtof(v.data(), nullptr); });
      }
      else if (*itr == "}")
      {
        stack--;
      }
      else if (*itr == "{")
      {
        stack++;
      }
      else
      {
      }
    }

    if (stack != 0 || *itr != "}")
    {
      OT_LOGF("group brace '}' error in lut ", lut.name);
    }

    return lut;
  }

  // Function: _extract_internal_power
  InternalPower Celllib::_extract_internal_power(token_iterator &itr, const token_iterator end)
  {

    InternalPower power;

    // Extract the lut template group
    if (itr = std::find(itr, end, "{"); itr == end)
    {
      OT_LOGF("can't find group brace '{' in timing");
    }

    int stack = 1;

    while (stack && ++itr != end)
    {

      if (*itr == "rise_power")
      {
        power.rise_power = _extract_lut(itr, end);
      }
      else if (*itr == "fall_power")
      { // Rise delay.
        power.fall_power = _extract_lut(itr, end);
      }
      else if (*itr == "related_pin")
      {

        if (++itr == end)
        {
          OT_LOGF("syntax error in related_pin");
        }

        power.related_pin = *itr;
      }
      else if (*itr == "}")
      {
        stack--;
      }
      else if (*itr == "{")
      {
        stack++;
      }
      else
      {
      }
    }

    if (stack != 0 || *itr != "}")
    {
      OT_LOGF("can't find group brace '}' in internal_power");
    }

    return power;
  }

  // Function: _extract_timing
  Timing Celllib::_extract_timing(token_iterator &itr, const token_iterator end, std::string pin)
  {

    Timing timing;

    // Extract the lut template group
    if (itr = std::find(itr, end, "{"); itr == end)
    {
      OT_LOGF("can't find group brace '{' in timing");
    }

    int stack = 1;

    while (stack && ++itr != end)
    {
      // std::cout << "timing: " << *itr << std::endl;
      if (*itr == "cell_fall")
      {
        timing.cell_fall = _extract_lut(itr, end);
      }
      else if (*itr == "cell_rise")
      { // Rise delay.
        timing.cell_rise = _extract_lut(itr, end);
      }
      else if (*itr == "fall_transition")
      { // Fall slew.
        timing.fall_transition = _extract_lut(itr, end);
      }
      else if (*itr == "rise_transition")
      { // Rise slew.
        timing.rise_transition = _extract_lut(itr, end);
      }
      else if (*itr == "rise_constraint")
      { // FF rise constraint.
        timing.rise_constraint = _extract_lut(itr, end);
      }
      else if (*itr == "fall_constraint")
      { // FF fall constraint.
        timing.fall_constraint = _extract_lut(itr, end);
      }
      else if (*itr == "timing_sense")
      { // Read the timing sense.

        OT_LOGF_IF(++itr == end, "syntex error in timing_sense");

        if (*itr == "negative_unate")
        {
          timing.sense = TimingSense::NEGATIVE_UNATE; // Negative unate.
        }
        else if (*itr == "positive_unate")
        { // Positive unate.
          timing.sense = TimingSense::POSITIVE_UNATE;
        }
        else if (*itr == "non_unate")
        { // Non unate.
          timing.sense = TimingSense::NON_UNATE;
        } // Massive kludge because some pdks have """" and some have ""
        else if (*itr == "negative_unate;")
        {
          timing.sense = TimingSense::NEGATIVE_UNATE;
        }
        else if (*itr == "positive_unate;")
        {
          timing.sense = TimingSense::POSITIVE_UNATE;
        }
        else if (*itr == "non_unate;")
        {
          timing.sense = TimingSense::NON_UNATE;
        }
        else
        {
          OT_LOGF("unexpected timing sense ", *itr);
        }
      }
      else if (*itr == "timing_type")
      {

        if (++itr == end)
        {
          OT_LOGF("syntax error in timing_type");
        }
        if (auto titr = timing_types.find(*itr); titr != timing_types.end())
        {

          timing.type = titr->second;
        }
        else
        {
          OT_LOGW("unexpected timing type ", *itr);
        }
      }

      else if (*itr == "related_pin")
      {

        if (++itr == end)
        {
          OT_LOGF("syntax error in related_pin");
        }

        timing.related_pin = *itr;
        // If we find the related pin, update all of the found ccsn stages
        // with the related pin, because this need not be in order unfortunately
        if (timing.ccsn_stages)
        {
          for (auto &ccsn_stage : *timing.ccsn_stages)
          {
            ccsn_stage.related_pin = timing.related_pin;
          }
        }
      }
      else if (*itr == "ccsn_last_stage")
      {
        // Extract CCSN stage information
        auto ccsn_stage = _extract_ccsn(itr, end);
        if (!timing.ccsn_stages)
        {
          timing.ccsn_stages = std::vector<CCSNStage>();
        }
        ccsn_stage.ccsn_stage_type = CCSNStageType::CCSN_LAST_STAGE;
        ccsn_stage.related_pin = timing.related_pin;
        ccsn_stage.pin = pin;

        timing.ccsn_stages->push_back(ccsn_stage);
      }
      else if (*itr == "ccsn_first_stage")
      {
        // Extract CCSN stage information
        auto ccsn_stage = _extract_ccsn(itr, end);
        if (!timing.ccsn_stages)
        {
          timing.ccsn_stages = std::vector<CCSNStage>();
        }
        ccsn_stage.ccsn_stage_type = CCSNStageType::CCSN_FIRST_STAGE;
        ccsn_stage.related_pin = timing.related_pin;
        ccsn_stage.pin = pin;

        timing.ccsn_stages->push_back(ccsn_stage);
      }
      else if (*itr == "ccsn_stage")
      {
        // Extract CCSN stage information
        auto ccsn_stage = _extract_ccsn(itr, end);
        if (!timing.ccsn_stages)
        {
          timing.ccsn_stages = std::vector<CCSNStage>();
        }
        ccsn_stage.ccsn_stage_type = CCSNStageType::CCSN_STAGE;
        ccsn_stage.related_pin = timing.related_pin;
        ccsn_stage.pin = pin;

        timing.ccsn_stages->push_back(ccsn_stage);
      }
      else if (*itr == "}")
      {
        stack--;
      }
      else if (*itr == "{")
      {
        stack++;
      }
      else
      {
      }
    }

    if (stack != 0 || *itr != "}")
    {
      OT_LOGF("can't find group brace '}' in timing");
    }

    return timing;
  }

  // Functoin: _extract_cellpin
  Cellpin Celllib::_extract_cellpin(token_iterator &itr, const token_iterator end)
  {

    Cellpin cellpin;

    if (itr = on_next_parentheses(
            itr,
            end,
            [&](auto &name) mutable
            { cellpin.name = name; });
        itr == end)
    {
      OT_LOGF("can't find cellpin name");
    }

    // Extract the lut template group
    if (itr = std::find(itr, end, "{"); itr == end)
    {
      OT_LOGF("can't find group brace '{' in cellpin ", cellpin.name);
    }

    // std::cout << "  -->" << cellpin.name << std::endl;

    int stack = 1;

    // std::cout << "cellpin: " << cellpin.name << std::endl;
    std::unordered_map<std::string, InternalPower> pending_internal_power;

    while (stack && ++itr != end)
    {
      // print itr

      if (*itr == "direction")
      {

        if (++itr == end)
        {
          OT_LOGF("can't get the direction in cellpin ", cellpin.name);
        }

        if (auto ditr = cellpin_directions.find(*itr); ditr != cellpin_directions.end())
        {
          cellpin.direction = ditr->second;
        }
        else
        {
          OT_LOGW("unexpected cellpin direction ", *itr);
        }
      }
      else if (*itr == "capacitance")
      {
        OT_LOGF_IF(++itr == end, "can't get the capacitance in cellpin ", cellpin.name);
        cellpin.capacitance = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "function")
      {
        OT_LOGF_IF(++itr == end, "can't get the function in cellpin ", cellpin.name);

        std::string func_str;

        // Collect tokens until we reach the semicolon
        for (; itr != end; ++itr)
        {
          if (*itr == ";")
          {
            break; // Stop at the semicolon
          }

          if (!func_str.empty())
          {
            func_str += " "; // Add space between tokens
          }
          func_str += std::string(*itr);
        }

        if (func_str.empty())
        {
          OT_LOGF("Unterminated function definition in cellpin ", cellpin.name);
        }

        cellpin.function = func_str;
      }

      else if (*itr == "max_capacitance")
      {
        OT_LOGF_IF(++itr == end, "can't get the max_capacitance in cellpin ", cellpin.name);
        cellpin.max_capacitance = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "min_capacitance")
      {
        OT_LOGF_IF(++itr == end, "can't get the min_capacitance in cellpin ", cellpin.name);
        cellpin.min_capacitance = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "max_transition")
      {
        OT_LOGF_IF(++itr == end, "can't get the max_transition in cellpin ", cellpin.name);
        cellpin.max_transition = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "min_transition")
      {
        OT_LOGF_IF(++itr == end, "can't get the min_transition in cellpin ", cellpin.name);
        cellpin.min_transition = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "fall_capacitance")
      {
        OT_LOGF_IF(++itr == end, "can't get fall_capacitance in cellpin ", cellpin.name);
        cellpin.fall_capacitance = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "rise_capacitance")
      {
        OT_LOGF_IF(++itr == end, "can't get rise_capacitance in cellpin ", cellpin.name);
        cellpin.rise_capacitance = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "fanout_load")
      {
        OT_LOGF_IF(++itr == end, "can't get fanout_load in cellpin ", cellpin.name);
        cellpin.fanout_load = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "max_fanout")
      {
        OT_LOGF_IF(++itr == end, "can't get max_fanout in cellpin ", cellpin.name);
        cellpin.max_fanout = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "min_fanout")
      {
        OT_LOGF_IF(++itr == end, "can't get min_fanout in cellpin ", cellpin.name);
        cellpin.min_fanout = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "clock")
      {
        OT_LOGF_IF(++itr == end, "can't get the clock status in cellpin ", cellpin.name);
        cellpin.is_clock = (*itr == "true") ? true : false;
      }
      else if (*itr == "original_pin")
      {
        OT_LOGF_IF(++itr == end, "can't get the original pin in cellpin ", cellpin.name);
        cellpin.original_pin = *itr;
      }
      // else if (*itr == "internal_power") {
      //     // Extract internal power information and store in map
      //     pending_internal_power.emplace(_extract_internal_power(itr, end).related_pin, _extract_internal_power(itr, end));
      // }
      else if (*itr == "ccsn_last_stage")
      {
        // Extract CCSN stage information
        auto ccsn_stage = _extract_ccsn(itr, end);
        if (!cellpin.ccsn_stages)
        {
          cellpin.ccsn_stages = std::vector<CCSNStage>();
        }
        ccsn_stage.ccsn_stage_type = CCSNStageType::CCSN_LAST_STAGE;
        ccsn_stage.pin = cellpin.name;
        cellpin.ccsn_stages->push_back(ccsn_stage);
      }
      else if (*itr == "ccsn_first_stage")
      {
        // Extract CCSN stage information
        auto ccsn_stage = _extract_ccsn(itr, end);
        if (!cellpin.ccsn_stages)
        {
          cellpin.ccsn_stages = std::vector<CCSNStage>();
        }
        ccsn_stage.ccsn_stage_type = CCSNStageType::CCSN_FIRST_STAGE;
        ccsn_stage.pin = cellpin.name;
        cellpin.ccsn_stages->push_back(ccsn_stage);
      }
      else if (*itr == "ccsn_stage")
      {
        // Extract CCSN stage information
        auto ccsn_stage = _extract_ccsn(itr, end);
        if (!cellpin.ccsn_stages)
        {
          cellpin.ccsn_stages = std::vector<CCSNStage>();
        }
        ccsn_stage.ccsn_stage_type = CCSNStageType::CCSN_STAGE;
        ccsn_stage.pin = cellpin.name;
        cellpin.ccsn_stages->push_back(ccsn_stage);
      }
      // else if (*itr == "internal_power")
      // {
      //   // Extract internal power information
      //   auto ip = _extract_internal_power(itr, end);
      //   cellpin.internal_power.push_back(ip);
      // }
      else if (*itr == "timing")
      {
        // Extract timing information
        auto ti = _extract_timing(itr, end, cellpin.name);
        // ot::printTiming(ti);
        cellpin.timings.push_back(ti);
        // std::cout << "Timing added, count: " << cellpin.timings.size() << std::endl;
      }
      else if (*itr == "}")
      {
        stack--;
      }
      else if (*itr == "{")
      {
        stack++;
      }
      else
      {
      }
    }

    if (stack != 0 || *itr != "}")
    {
      OT_LOGF("can't find group brace '}' in cellpin ", cellpin.name);
    }

    return cellpin;
  }

  // Function: _extract_cell
  Cell Celllib::_extract_cell(token_iterator &itr, const token_iterator end)
  {

    Cell cell;

    if (itr = on_next_parentheses(
            itr,
            end,
            [&](auto &name) mutable
            { cell.name = name; });
        itr == end)
    {
      OT_LOGF("can't find cell name");
    }

    // Extract the lut template group
    if (itr = std::find(itr, end, "{"); itr == end)
    {
      OT_LOGF("can't find group brace '{' in cell ", cell.name);
    }

    int stack = 1;

    while (stack && ++itr != end)
    {
      if (*itr == "ff")
      {
        OT_LOGF_IF(++itr == end, "syntax error in ff");
        cell.sequential_info = _extract_sequential_info(itr, end);
        cell.is_sequential = true;
        cell.is_ff = true;
      }
      else if (*itr == "latch")
      {
        OT_LOGF_IF(++itr == end, "syntax error in latch");
        cell.sequential_info = _extract_sequential_info(itr, end);
        cell.is_sequential = true;
        cell.is_ff = false;
      }
      else if (*itr == "cell_leakage_power")
      { // Read the leakage power.
        OT_LOGF_IF(++itr == end, "can't get leakage power in cell ", cell.name);
        cell.leakage_power = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "cell_footprint")
      { // Read the footprint.
        OT_LOGF_IF(++itr == end, "can't get footprint in cell ", cell.name);
        cell.cell_footprint = *itr;
      }
      else if (*itr == "area")
      { // Read the area.
        OT_LOGF_IF(++itr == end, "can't get area in cell ", cell.name);
        cell.area = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "pin")
      { // Read the cell pin group.
        auto pin = _extract_cellpin(itr, end);

        // Debug before insertion
        // std::cout << "[DEBUG] Before insertion: Pin '" << pin.name
        //           << "' has " << pin.timings.size() << " timings." << std::endl;

        auto [it, inserted] = cell.cellpins.emplace(pin.name, std::move(pin));

        if (!inserted)
        {
          // std::cout << "[DEBUG] Merging timings for existing pin: '"
          //           << it->second.name << "'." << std::endl;

          // Merge timing information if the pin already exists
          it->second.timings.insert(
              it->second.timings.end(),
              std::make_move_iterator(pin.timings.begin()),
              std::make_move_iterator(pin.timings.end()));
        }

        // // Debug after insertion
        // std::cout << "[DEBUG] After insertion: Pin '" << it->second.name
        //           << "' now has " << it->second.timings.size() << " timings." << std::endl;
      }

      else if (*itr == "}")
      {
        stack--;
      }
      else if (*itr == "{")
      {
        stack++;
      }
      else
      {
        // OT_LOGW("unexpected token ", *itr);
      }
    }

    if (stack != 0 || *itr != "}")
    {
      OT_LOGF("can't find group brace '}' in cell ", cell.name);
    }
    // std::cout << "Cell added: " << cell.name << std::endl;
    return cell;
  }

  Wireload Celllib::_extract_wireload(token_iterator &itr, const token_iterator end)
  {

    Wireload wl;

    if (itr = on_next_parentheses(
            itr,
            end,
            [&](auto &name) mutable
            { wl.name = name; });
        itr == end)
    {
      OT_LOGF("can't find wireload name");
    }

    // Extract the lut template group
    if (itr = std::find(itr, end, "{"); itr == end)
    {
      OT_LOGF("can't find group brace '{' in wireload ", wl.name);
    }

    int stack = 1;

    while (stack && ++itr != end)
    {
      if (*itr == "capacitance")
      {
        OT_LOGF_IF(++itr == end, "can't get capacitance in wireload ", wl.name);
        wl.capacitance = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "slope")
      {
        OT_LOGF_IF(++itr == end, "can't get slope in wireload ", wl.name);
        wl.slope = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "resistance")
      {
        OT_LOGF_IF(++itr == end, "can't get resistance in wireload ", wl.name);
        wl.resistance = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "}")
      {
        stack--;
      }
      else if (*itr == "{")
      {
        stack++;
      }
      else
      {
      }
    }

    if (stack != 0 || *itr != "}")
    {
      OT_LOGF("can't find group brace '}' in wireload ", wl.name);
    }

    return wl;
  }

  // Function: _extract_ccsn
  CCSNStage Celllib::_extract_ccsn(token_iterator &itr, const token_iterator end)
  {
    CCSNStage ccsn_stage;

    if (itr = on_next_parentheses(itr, end, [&](auto &name)
                                  { ccsn_stage.stage_type = name; });
        itr == end)
    {
      OT_LOGF("can't find CCSN stage type");
    }

    // Extract the CCSN stage group
    if (itr = std::find(itr, end, "{"); itr == end)
    {
      OT_LOGF("can't find group brace '{' in CCSN stage");
    }

    int stack = 1;

    while (stack && ++itr != end)
    {
      // std::cout << "processing line " << itr->data() << std::endl;
      if (*itr == "dc_current")
      {
        ccsn_stage.dc_current.push_back(_extract_lut(itr, end));
      }
      else if (*itr == "output_voltage_fall")
      {
        ccsn_stage.output_voltage_fall.push_back(_extract_lut(itr, end));
      }
      else if (*itr == "output_voltage_rise")
      {
        ccsn_stage.output_voltage_rise.push_back(_extract_lut(itr, end));
      }
      else if (*itr == "propagated_noise_high")
      {
        ccsn_stage.propagated_noise_high.push_back(_extract_lut(itr, end));
      }
      else if (*itr == "propagated_noise_low")
      {
        ccsn_stage.propagated_noise_low.push_back(_extract_lut(itr, end));
      }
      else if (*itr == "is_inverting")
      {
        OT_LOGF_IF(++itr == end, "can't get is_inverting in CCSN stage");

        if (*itr == "true")
        {
          ccsn_stage.is_inverting = true;
        }
        else if (*itr == "false")
        {
          ccsn_stage.is_inverting = false;
        }
        else if (*itr == "true;")
        {
          ccsn_stage.is_inverting = true;
        }
        else if (*itr == "false;")
        {
          ccsn_stage.is_inverting = false;
        }
        else
        {
          OT_LOGF("unexpected is_inverting value in CCSN stage: ", *itr);
        }
      }
      else if (*itr == "is_needed")
      {
        OT_LOGF_IF(++itr == end, "can't get is_needed in CCSN stage");
        if (*itr == "true")
        {
          ccsn_stage.is_needed = true;
        }
        else if (*itr == "false")
        {
          ccsn_stage.is_needed = false;
        }
        else if (*itr == "true;")
        {
          ccsn_stage.is_needed = true;
        }
        else if (*itr == "false;")
        {
          ccsn_stage.is_needed = false;
        }
        else if (*itr == ";")
        {
          // IDK if this is a good idea but I want speed.
        }
        else
        {
          OT_LOGF("unexpected is_needed value in CCSN stage: ", *itr);
        }
      }
      else if (*itr == "miller_cap_fall")
      {
        OT_LOGF_IF(++itr == end, "can't get miller_cap_fall in CCSN stage");
        ccsn_stage.miller_cap_fall = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "miller_cap_rise")
      {
        OT_LOGF_IF(++itr == end, "can't get miller_cap_rise in CCSN stage");
        ccsn_stage.miller_cap_rise = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "pin")
      {
        OT_LOGF_IF(++itr == end, "can't get pin in CCSN stage");
        ccsn_stage.pin = *itr;
      }
      else if (*itr == "related_pin")
      {
        OT_LOGF_IF(++itr == end, "can't get related_pin in CCSN stage");
        ccsn_stage.related_pin = *itr;
      }
      else if (*itr == "when")
      {
        OT_LOGF_IF(++itr == end, "can't get when condition in CCSN stage");
        ccsn_stage.when = *itr;
      }

      else if (*itr == "stage_type")
      {
        OT_LOGF_IF(++itr == end, "can't get stage_type in CCSN stage");
        ccsn_stage.stage_type = *itr;
      }
      else if (*itr == "}")
      {
        stack--;
      }
      else if (*itr == "{")
      {
        stack++;
      }
      else if (*itr == ";")
      {
        // IDK if this is a good idea but I want speed.
      }
      else
      {
        OT_LOGW("unexpected token in CCSN stage: ", *itr);
      }
    }

    if (stack != 0 || *itr != "}")
    {
      OT_LOGF("can't find group brace '}' in CCSN stage");
    }

    return ccsn_stage;
  }

  // Procedure: read
  void Celllib::read(const std::filesystem::path &path)
  {

    std::ifstream ifs(path, std::ios::ate);

    // return on failure
    OT_LOGF_IF(!ifs, "failed to open celllib ", path);

    size_t fsize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<char> buffer(fsize + 1);
    ifs.read(buffer.data(), fsize);
    buffer[fsize] = 0;

    // get tokens
    std::vector<std::string> tokens;
    tokens.reserve(buffer.size() / sizeof(std::string));

    _uncomment(buffer);
    _tokenize(buffer, tokens);

    // Set up the iterator
    auto itr = tokens.begin();
    auto end = tokens.end();

    // Read the library name.
    if (itr = std::find(itr, end, "library"); itr == end)
    {
      OT_LOGF("can't find keyword ", std::quoted("library"));
    }

    if (itr = on_next_parentheses(
            itr,
            end,
            [&](auto &str) mutable
            { name = str; });
        itr == end)
    {
      OT_LOGF("can't find library name");
    }

    // Extract the library group
    if (itr = std::find(itr, tokens.end(), "{"); itr == tokens.end())
    {
      OT_LOGF("can't find library group symbol '{'");
    }

    int stack = 1;

    while (stack && ++itr != end)
    {

      if (*itr == "lu_table_template")
      {
        auto lut = _extract_lut_template(itr, end);
        lut_templates[lut.name] = lut;
      }
      else if (*itr == "power_lut_template")
      {
        auto lut = _extract_lut_template(itr, end);
        lut_templates[lut.name] = lut;
      }
      else if (*itr == "delay_model")
      {
        OT_LOGF_IF(++itr == end, "syntax error in delay_model");
        if (auto ditr = delay_models.find(*itr); ditr != delay_models.end())
        {
          delay_model = ditr->second;
        }
        else
        {
          OT_LOGW("unexpected delay model ", *itr);
        }
      }
      else if (*itr == "nom_process")
      {
        OT_LOGF_IF(++itr == end, "syntax error in nom_process");
        nom_process = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "nom_temperature")
      {
        OT_LOGF_IF(++itr == end, "syntax error in nom_temperature");
        nom_temperature = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "nom_voltage")
      {
        OT_LOGF_IF(++itr == end, "syntax error in nom_voltage");
        nom_voltage = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "default_cell_leakage_power")
      {
        OT_LOGF_IF(++itr == end, "syntax error in default_cell_leakage_power");
        default_cell_leakage_power = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "default_inout_pin_cap")
      {
        OT_LOGF_IF(++itr == end, "syntax error in default_inout_pin_cap");
        default_inout_pin_cap = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "default_input_pin_cap")
      {
        OT_LOGF_IF(++itr == end, "syntax error in default_input_pin_cap");
        default_input_pin_cap = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "default_output_pin_cap")
      {
        OT_LOGF_IF(++itr == end, "syntax error in default_output_pin_cap");
        default_output_pin_cap = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "default_fanout_load")
      {
        OT_LOGF_IF(++itr == end, "syntax error in default_fanout_load");
        default_fanout_load = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "default_max_fanout")
      {
        OT_LOGF_IF(++itr == end, "syntax error in default_max_fanout");
        default_max_fanout = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "default_max_transition")
      {
        OT_LOGF_IF(++itr == end, "syntax error in default_max_transition");
        default_max_transition = std::strtof(itr->data(), nullptr);
      }
      else if (*itr == "operating_conditions")
      {
        OT_LOGF_IF(++itr == end, "syntax error in operating_conditions");
        voltage = _extract_operating_conditions(itr, end);
        // TODO: Unit field.
      }
      else if (*itr == "time_unit")
      {
        OT_LOGF_IF(++itr == end, "time_unit syntax error");
        time_unit = make_time_unit(*itr);
      }
      else if (*itr == "voltage_unit")
      {
        OT_LOGF_IF(++itr == end, "voltage_unit syntax error");
        voltage_unit = make_voltage_unit(*itr);
      }
      else if (*itr == "current_unit")
      {
        OT_LOGF_IF(++itr == end, "current_unit syntax error");
        current_unit = make_current_unit(*itr);
      }
      else if (*itr == "pulling_resistance_unit")
      {
        OT_LOGF_IF(++itr == end, "pulling_resistance_unit syntax error");
        resistance_unit = make_resistance_unit(*itr);
      }
      else if (*itr == "leakage_power_unit")
      {
        OT_LOGF_IF(++itr == end, "leakage_power_unit syntax error");
        power_unit = make_power_unit(*itr);
      }
      else if (*itr == "capacitive_load_unit")
      {
        std::string unit;
        if (itr = on_next_parentheses(
                itr,
                end,
                [&](auto &str) mutable
                { unit += str; });
            itr == end)
        {
          OT_LOGF("capacitive_load_unit syntax error");
        }
        capacitance_unit = make_capacitance_unit(unit);
      }
      else if (*itr == "wire_load")
      {
        // std::cout << "wire_load" << std::endl;
        auto wl = _extract_wireload(itr, end);
        wireloads[wl.name] = wl;
      }
      else if (*itr == "cell")
      {
        auto cell = _extract_cell(itr, end);
        cells[cell.name] = std::move(cell);
      }
      else if (*itr == "}")
      {
        stack--;
      }
      else if (*itr == "{")
      {
        stack++;
      }
      else
      {
      }
    }

    if (stack != 0 || *itr != "}")
    {
      OT_LOGF("can't find library group brace '}'");
    }

    // iterate thru the timings and print cell name then printTiming()
    //  for(auto& ckvp : cells) {
    //    auto& cell = ckvp.second;
    //    for(auto& pkvp : cell.cellpins) {
    //      auto& cpin = pkvp.second;
    //      for(auto& t : cpin.timings) {
    //        ot::printTiming(t);
    //      }
    //    }
    //  }
    //  for (auto& cell : cells) {
    //    std::cout << "Cell: " << cell.first << std::endl;
    //    for (auto& cellpin : cell.second.cellpins) {
    //        std::cout << "  Cellpin: " << cellpin.first << std::endl;
    //        for (auto& timing : cellpin.second.timings) {
    //            std::cout << "    Timing: " << timing << std::endl;
    //        }
    //    }
    //  }

    _apply_default_values();
  }

  // Procedure: _apply_default_values
  void Celllib::_apply_default_values()
  {

    for (auto &ckvp : cells)
    {

      auto &cell = ckvp.second;

      // apply the default leakage power
      if (!cell.leakage_power)
      {
        cell.leakage_power = default_cell_leakage_power;
      }

      for (auto &pkvp : cell.cellpins)
      {

        auto &cpin = pkvp.second;

        // direction-specific default values
        if (!cpin.direction)
        {
          OT_LOGW("cellpin ", cell.name, '/', cpin.name, " has no direction defined");
          continue;
        }

        switch (*cpin.direction)
        {

        case CellpinDirection::INPUT:
          if (!cpin.capacitance)
          {
            cpin.capacitance = default_input_pin_cap;
          }

          if (!cpin.fanout_load)
          {
            cpin.fanout_load = default_fanout_load;
          }
          break;

        case CellpinDirection::OUTPUT:
          if (!cpin.capacitance)
          {
            cpin.capacitance = default_output_pin_cap;
          }

          if (!cpin.max_fanout)
          {
            cpin.max_fanout = default_max_fanout;
          }

          if (!cpin.max_transition)
          {
            cpin.max_transition = default_max_transition;
          }
          break;

        case CellpinDirection::INOUT:
          if (!cpin.capacitance)
          {
            cpin.capacitance = default_inout_pin_cap;
          }
          break;

        case CellpinDirection::INTERNAL:
          break;
        }
      }
    }
  }

  // Procedure: scale_time
  // Convert the numerics to the new unit
  void Celllib::scale_time(float s)
  {

    if (default_max_transition)
    {
      default_max_transition = *default_max_transition * s;
    }

    for (auto &c : cells)
    {
      c.second.scale_time(s);
    }
  }

  // Procedure: scale_capacitance
  void Celllib::scale_capacitance(float s)
  {

    if (default_inout_pin_cap)
    {
      default_inout_pin_cap = *default_inout_pin_cap * s;
    }

    if (default_input_pin_cap)
    {
      default_input_pin_cap = *default_input_pin_cap * s;
    }

    if (default_output_pin_cap)
    {
      default_output_pin_cap = *default_output_pin_cap * s;
    }

    for (auto &c : cells)
    {
      c.second.scale_capacitance(s);
    }
  }

  // Procedure: scale_voltage
  void Celllib::scale_voltage(float s)
  {
    // TODO
  }

  // Procedure: scale_current
  void Celllib::scale_current(float s)
  {

    // TODO
  }

  // Procedure: scale_resistance
  void Celllib::scale_resistance(float s)
  {
    // TODO
  }

  // Procedure: scale_power
  void Celllib::scale_power(float s)
  {
    // TODO
  }

  // Operator: <<
  std::ostream &operator<<(std::ostream &os, const Celllib &c)
  {

    // Write the comment.
    os << "/* Generated by OpenTimer " << " */\n";

    // Write library name.
    os << "library (\"" << c.name << "\") {\n\n";

    // Delay modeA
    if (c.delay_model)
    {
      os << "delay_model : " << to_string(*(c.delay_model)) << ";\n";
    }

    // Library units
    if (auto u = c.time_unit; u)
    {
      os << "time_unit : \"" << u->value() << "s\"\n";
    }

    if (auto u = c.voltage_unit; u)
    {
      os << "voltage_unit : \"" << u->value() << "V\"\n";
    }

    if (auto u = c.current_unit; u)
    {
      os << "current_unit : \"" << u->value() << "A\"\n";
    }

    if (auto u = c.resistance_unit; u)
    {
      os << "pulling_resistance_unit : \"" << u->value() << "ohm\"\n";
    }

    if (auto u = c.power_unit; u)
    {
      os << "leakage_power_unit : \"" << u->value() << "W\"\n";
    }

    if (auto u = c.capacitance_unit; u)
    {
      os << "capacitive_load_unit (" << u->value() << ",F)\"\n";
    }

    // default values
    if (c.default_cell_leakage_power)
    {
      os << *c.default_cell_leakage_power << '\n';
    }

    if (c.default_inout_pin_cap)
    {
      os << *c.default_inout_pin_cap << '\n';
    }

    if (c.default_input_pin_cap)
    {
      os << *c.default_input_pin_cap << '\n';
    }

    if (c.default_output_pin_cap)
    {
      os << *c.default_fanout_load << '\n';
    }

    if (c.default_max_fanout)
    {
      os << *c.default_max_fanout << '\n';
    }

    if (c.default_max_transition)
    {
      os << *c.default_max_transition << '\n';
    }

    // Write the lut templates
    for (const auto &kvp : c.lut_templates)
    {
      os << kvp.second << '\n';
    }

    // Write all cells.
    for (const auto &kvp : c.cells)
    {
      os << kvp.second << '\n';
    }

    // Write library ending group symbol.
    os << "}\n";

    return os;
  }

}; // namespace ot. ------------------------------------------------------------------------------
