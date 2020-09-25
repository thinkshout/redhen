<?php

namespace Drupal\redhen\Routing;

use Drupal\Core\Routing\RouteSubscriberBase;
use Symfony\Component\Routing\RouteCollection;

/**
 * Set Admin Route options.
 *
 * @package Drupal\redhen\Routing
 */
class RouteSubscriber extends RouteSubscriberBase {

  /**
   * {@inheritdoc}
   */
  protected function alterRoutes(RouteCollection $collection) {
    if (\Drupal::config('redhen.settings')->get('redhen_admin_path')) {
      $redhen_routes = [
        'redhen.dashboard',
        'entity.redhen_contact.canonical',
        'entity.redhen_connection.canonical',
        'entity.redhen_org.canonical',
      ];
      foreach ($redhen_routes as $routename) {
        $route = $collection->get($routename);
        if ($route) {
          $route->setOption('_admin_route', TRUE);
        }
      }
    }
  }
}
