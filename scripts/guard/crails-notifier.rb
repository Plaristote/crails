module Crails
  module Notifier
    def self.notify title, message
      notifier = notifiers.first
      notifiers.first.notify title, message unless notifier.nil?
      puts ">> " + message.magenta
    end

    def self.notifiers
      [
        NotifySend, KDialog, TerminalNotifier
      ].select {|i| i.is_available? }
    end

    class NotifySend
      def self.is_available?
        `which notify-send 2> /dev/null`
        $?.success?
      end

      def self.notify title, message
        `notify-send "#{message}" -t 2500`
      end
    end

    class KDialog
      def self.is_available?
        `which kdialog 2> /dev/null`
        $?.success?
      end

      def self.notify title, message
        `kdialog --title "#{title}" --passivepopup "#{message}"`
      end
    end

    class TerminalNotifier
      def self.is_available?
        `which terminal-notifier 2> /dev/null`
        $?.success?
      end

      def self.notify title, message
        `terminal-notifier -title "#{title}" -message "#{message}"`
      end
    end
  end
end

class String
  def black;          "\033[30m#{self}\033[0m" end
  def red;            "\033[31m#{self}\033[0m" end
  def green;          "\033[32m#{self}\033[0m" end
  def brown;          "\033[33m#{self}\033[0m" end
  def blue;           "\033[34m#{self}\033[0m" end
  def magenta;        "\033[35m#{self}\033[0m" end
  def cyan;           "\033[36m#{self}\033[0m" end
  def gray;           "\033[37m#{self}\033[0m" end
  def bg_black;       "\033[40m#{self}\033[0m" end
  def bg_red;         "\033[41m#{self}\033[0m" end
  def bg_green;       "\033[42m#{self}\033[0m" end
  def bg_brown;       "\033[43m#{self}\033[0m" end
  def bg_blue;        "\033[44m#{self}\033[0m" end
  def bg_magenta;     "\033[45m#{self}\033[0m" end
  def bg_cyan;        "\033[46m#{self}\033[0m" end
  def bg_gray;        "\033[47m#{self}\033[0m" end
  def bold;           "\033[1m#{self}\033[22m" end
  def reverse_color;  "\033[7m#{self}\033[27m" end
end
