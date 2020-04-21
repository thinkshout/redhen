<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\Core\Entity\Routing\DefaultHtmlRouteProvider;
use Symfony\Component\Routing\Route;

/**
 * Provides routes for Connection entities.
 *
 * @see Drupal\Core\Entity\Routing\AdminHtmlRouteProvider
 * @see Drupal\Core\Entity\Routing\DefaultHtmlRouteProvider
 */
class ConnectionHtmlRouteProvider extends DefaultHtmlRouteProvider {

  /**
   * {@inheritdoc}
   */
  public function getRoutes(EntityTypeInterface $entity_type) {
    $collection = parent::getRoutes($entity_type);

    $entity_type_id = $entity_type->id();

    if ($collection_route = $this->getCollectionRoute($entity_type)) {
      $collection->add("entity.{$entity_type_id}.collection", $collection_route);
    }

    // Get all connection form and add routes.
    if ($add_entity_page_routes = $this->getAddEntityRoutes($entity_type)) {
      foreach ($add_entity_page_routes as $entity_type_key => $entity_page_routes) {
        if ($entity_page_routes['form']) {
          $collection->add("$entity_type_key.connection.add_form", $entity_page_routes['form']);
        }
        if ($entity_page_routes['add']) {
          $collection->add("$entity_type_key.connection.add_page", $entity_page_routes['add']);
        }
      }
    }

    if ($settings_form_route = $this->getSettingsFormRoute($entity_type)) {
      $collection->add("$entity_type_id.settings", $settings_form_route);
    }

    return $collection;
  }

  /**
   * Gets the collection route.
   *
   * @param \Drupal\Core\Entity\EntityTypeInterface $entity_type
   *   The entity type.
   *
   * @return \Symfony\Component\Routing\Route|null
   *   The generated route, if available.
   */
  protected function getCollectionRoute(EntityTypeInterface $entity_type) {
    if ($entity_type->hasLinkTemplate('collection') && $entity_type->hasListBuilderClass()) {
      $entity_type_id = $entity_type->id();
      $route = new Route($entity_type->getLinkTemplate('collection'));
      $route
        ->setDefaults([
          '_entity_list' => $entity_type_id,
          '_title' => "{$entity_type->getLabel()} list",
        ])
        ->setRequirement('_permission', 'view active connection entities+view inactive connection entities')
        ->setOption('_admin_route', TRUE);

      return $route;
    }
  }

  /**
   * Gets the add-form route.
   *
   * @param \Drupal\Core\Entity\EntityTypeInterface $entity_type
   *   The entity type.
   *
   * @return \Symfony\Component\Routing\Route|null
   *   The generated route, if available.
   */
  protected function getAddFormRoute(EntityTypeInterface $entity_type) {
    if ($entity_type->hasLinkTemplate('add-form')) {
      $entity_type_id = $entity_type->id();
      $parameters = [
        $entity_type_id => ['type' => 'entity:' . $entity_type_id],
        'redhen_type' => ['type' => 'redhen_type'],
        'entity' => ['type' => 'entity:{redhen_type}'],
      ];

      $route = new Route($entity_type->getLinkTemplate('add-form'));
      $bundle_entity_type_id = $entity_type->getBundleEntityType();
      // Content entities with bundles are added via a dedicated controller.
      $route
        ->setDefaults([
          '_controller' => 'Drupal\redhen_connection\Controller\ConnectionAddController::addForm',
          '_title_callback' => 'Drupal\redhen_connection\Controller\ConnectionAddController::getAddFormTitle',
        ])
        ->setRequirement('_entity_create_access', $entity_type_id . ':{' . $bundle_entity_type_id . '}');
      $parameters[$bundle_entity_type_id] = ['type' => 'entity:' . $bundle_entity_type_id];

      $route
        ->setOption('parameters', $parameters);

      return $route;
    }
  }

  /**
   * Gets all entity type add connection routes.
   *
   * @param \Drupal\Core\Entity\EntityTypeInterface $connection_entity_type
   *   The entity type.
   *
   * @return array
   *   An array of routes keyed with "form" and "add", if available.
   */
  protected function getAddEntityRoutes(EntityTypeInterface $connection_entity_type) {
    $routes = [];
    $connection_entity_type_id = $connection_entity_type->id();
    $service = \Drupal::service('redhen_connection.connections');
    $entity_types = $service->getAllConnectionEntityTypes();

    // Iterate over each entity type to find connectable entities.
    foreach ($entity_types as $type) {
      if ($canonical = $type->getLinkTemplate('canonical')) {
        $type_id = $type->id();

        // Skip over redhen_connection entity types.
        if ($type_id == 'redhen_connection_type' || $type_id == 'redhen_connection_role') {
          continue;
        }

        // Build route parameters.
        $parameters = [
          $type_id => ['type' => 'entity:' . $type_id],
        ];

        $route[$type_id] = [];

        // Add both form and add routes to routes array.
        foreach (['form', 'add'] as $route_type) {
          // Set specific values for form routes.
          if ($route_type === 'form') {
            $path = '/connection/add/{redhen_connection_type}';
            $parameters[$connection_entity_type_id] = ['type' => 'entity:' . $connection_entity_type_id];
            $controller = 'Drupal\redhen_connection\Controller\ConnectionAddController::addForm';
            $title_callback = 'Drupal\redhen_connection\Controller\ConnectionAddController::getAddFormTitle';
            $routes[$type_id]['form'] = $this->generateRoute($canonical, $path, $parameters, $controller, $title_callback, $connection_entity_type_id);
          }
          else {
            $path = "/connection/add";
            $controller = 'Drupal\redhen_connection\Controller\ConnectionAddController::add';
            $title_callback = 'Drupal\redhen_connection\Controller\ConnectionAddController::getAddTitle';
            $routes[$type_id]['add'] = $this->generateRoute($canonical, $path, $parameters, $controller, $title_callback, $connection_entity_type_id);
          }
        }
      }
    }

    return $routes;
  }

  /**
   * Gets the settings form route.
   *
   * @param \Drupal\Core\Entity\EntityTypeInterface $entity_type
   *   The entity type.
   *
   * @return \Symfony\Component\Routing\Route|null
   *   The generated route, if available.
   */
  protected function getSettingsFormRoute(EntityTypeInterface $entity_type) {
    if (!$entity_type->getBundleEntityType()) {
      $route = new Route("/admin/structure/{$entity_type->id()}/settings");
      $route
        ->setDefaults([
          '_title' => "{$entity_type->getLabel()} settings",
        ])
        ->setRequirement('_permission', $entity_type->getAdminPermission())
        ->setOption('_admin_route', TRUE);

      return $route;
    }
  }

  /**
   * Generates a route based on the provided values.
   *
   * @param string $canonical
   *   Canonical path of entity type.
   * @param string $path
   *   Path for this route.
   * @param array $parameters
   *   Parameters for the route.
   * @param string $controller
   *   Controller method for the route.
   * @param string $title_callback
   *   Title callback for the route.
   * @param string $connection_entity_type_id
   *   Connection entity type machine name.
   *
   * @return \Symfony\Component\Routing\Route
   *   Drupal route object.
   */
  protected function generateRoute($canonical, $path, array $parameters, $controller, $title_callback, $connection_entity_type_id) {
    $route = new Route($canonical . $path);
    $route->setOption('parameters', $parameters);
    $route
      ->setDefaults([
        '_controller' => $controller,
        '_title_callback' => $title_callback,
      ])
      ->setOption('_admin_route', TRUE)
      ->setRequirement('_entity_create_access', $connection_entity_type_id);
    return $route;
  }

}
