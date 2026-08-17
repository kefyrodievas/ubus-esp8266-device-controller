include $(TOPDIR)/rules.mk

PKG_NAME:=esp_controller
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

CMAKE_INSTALL:=1


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/esp_controller
	CATEGORY:=Base system
	TITLE:=esp_controller
	DEPENDS:=+libserialport +libblobmsg-json +libubus +libubox
endef

define Package/esp_controller/description
	Connects the router to a specially programmed ESP8266 controller via serial
endef

define Package/esp_controller/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/esp_controller $(1)/usr/bin
	$(INSTALL_BIN) ./files/esp_controller.init $(1)/etc/init.d/esp_controller
endef

$(eval $(call BuildPackage,esp_controller))
