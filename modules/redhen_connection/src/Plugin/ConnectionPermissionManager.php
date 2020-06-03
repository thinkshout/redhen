<?php

namespace Drupal\redhen_connection\Plugin;

use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides the Connection permission plugin manager.
 */
class ConnectionPermissionManager extends DefaultPluginManager {

  /**
   * Constructs a new ConnectionPermissionManager object.
   *
   * @param \Traversable $namespaces
   *   An object that implements \Traversable which contains the root paths
   *   keyed by the corresponding namespace to look for plugin implementations.
   * @param \Drupal\Core\Cache\CacheBackendInterface $cache_backend
   *   Cache backend instance to use.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler to invoke the alter hook with.
   */
  public function __construct(\Traversable $namespaces, CacheBackendInterface $cache_backend, ModuleHandlerInterface $module_handler) {
    parent::__construct('Plugin/ConnectionPermission', $namespaces, $module_handler, 'Drupal\redhen_connection\Plugin\ConnectionPermissionInterface', 'Drupal\redhen_connection\Annotation\ConnectionPermission');

    $this->alterInfo('redhen_connection_connection_permission_info');
    $this->setCacheBackend($cache_backend, 'redhen_connection_connection_permission_plugins');
  }

}
