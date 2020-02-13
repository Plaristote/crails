 class String
  def camelize
    parts = self.split(/_+|-+|\W+/)
    (parts.map do |part| part.capitalize end).join
  end

  def dasherize
    tmp = self.gsub(/([A-Z]+)/, '_\1')
    parts = tmp.split(/_+|-+|\W+/)
    parts.select! {|item| item != ""}
    (parts.map do |part| part.downcase end).join '-'
  end
end

#module CrailsCheerpHtml
  def find_anchorable_anchor el
    it = el
    loop do
      it = it.previous
      break if it.nil? || it.name != "text" || it.name == "comment"
    end
    unless it.nil?
      { el: it, mode: :append }
    else
      it = el
      loop do
        it = it.next
        break if it.nil? || it.name != "text" || it.name == "comment"
      end
      unless it.nil?
        { el: it, mode: :prepend }
      else
        { el: el.parent, mode: :children }
      end
    end
  end

  def anchor_symbol_to_enum anchor_name
    "Crails::Front::" + case anchor_name
    when :append   then "AppendAnchor"
    when :prepend  then "PrependAnchor"
    when :children then "ChildrenAnchor"
    end
  end  
  
  def has_trigger_attributes? el
    result = false
    el.attributes.each do |key,value|
      result = true if key.end_with?(".trigger")
    end
    result
  end

  def make_attr_from_el el
    hard_attributes  = []
    el.attributes.each do |key, value|
      reserved_keywords = ["ref", "slot", "_cheerp_class", "_cheerp_ref"]
      if !reserved_keywords.include?(key) && !(key.end_with? ".bind") && !(key.end_with? ".trigger") && !(key.end_with? ".for")
        hard_attributes << "{\"#{key}\",\"#{value}\"}"
      end
    end
    hard_attributes
  end
  
  def extract_bind_mode_from value
    bind_mode_match = value.to_s.match(/\s*&\s*(throttle|signal):([a-zA-Z0-9_-]+)$/)
    if bind_mode_match.nil?
      bind_mode = ""
    else
      value = value.to_s.delete_suffix bind_mode_match[0]
      bind_mode_enum = case bind_mode_match[1]
                       when 'signal' then "SignalBind"
                       when 'throttle' then "ThrottleBind"
                       else "StaticBind"
                       end
      bind_mode = ".use_mode(Crails::Front::Bindable::#{bind_mode_enum}, \"#{bind_mode_match[2]}\")"
    end
    [value, bind_mode]
  end
#end
