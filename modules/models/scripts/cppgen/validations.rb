def validate_number_min name, min
  code = <<CPP
if (#{name} < #{min})
{
  errors["#{name}"]["t"] = "validate.min";
  errors["#{name}"]["params"]["min"] = #{min};
  errors["#{name}"]["params"]["val"] = #{name};
}
CPP
end

def validate_number_max name, max
  code = <<CPP
if (#{name} < #{max})
{
  errors["#{name}"]["t"] = "validate.max";
  errors["#{name}"]["params"]["max"] = #{min};
  errors["#{name}"]["params"]["val"] = #{name};
}
CPP
end

def validate_uniqueness source_type, name
  if source_type.start_with?("std::shared_ptr")
    type = source_type[16..-2]
<<CPP
  {
    auto& database = *#{GeneratorBase.odb_connection[:object]}::instance;
    odb::result<#{source_type}> results;

    database.find(results, odb::query<#{source_type}::#{name}->id == get_#{name}_id());
    if (results.size() > 0)
      errors["#{name}"]["t"] = "validate.uniqueness";
  }
CPP
  else
<<CPP
  {
    auto& database = *#{GeneratorBase.odb_connection[:object]}::instance;
    odb::result<#{source_type}> results;

    database.find(results, odb::query<#{source_type}::#{name} == #{name});
    if (results.size() > 0)
      errors["#{name}"]["t"] = "validate.uniqueness";
  }
CPP
  end
end

def validate_required type, name
  if type == "std::string"
<<CPP
if (#{name} == "")
  errors["#{name}"]["t"] = "validate.required";
CPP
  elsif type.start_with?("std::shared_ptr")
<<CPP
if (#{name} == nullptr)
  errors["#{name}_id"]["t"] = "validate.required";
CPP
  elsif type == "ODB::id_type"
<<CPP
if (#{name} == 0)
  errors["#{name}"]["t"] = "validate.required";
CPP
  else
    ""
  end
end

def validate_self_reference type, name
  raw_ptr_type = type.gsub /std::shared_ptr<(.*)>/, '\1*'
<<CPP
if (#{name} != nullptr && #{name}->get_id() == static_cast<#{raw_ptr_type}>(this)->get_id())
  errors["#{name}"]["t"] = "validate.self-reference";
CPP
end
