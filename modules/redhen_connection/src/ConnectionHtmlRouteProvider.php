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

    if ($add_entity_add_page_routes = $this->getAddEntityPageRoutes($entity_type)) {
      foreach ($add_entity_add_page_routes as $entity_type_key => $entity_page_route) {
        $collection->add("$entity_type_key.connection.add_page", $entity_page_route);
      }
    }

    if ($add_entity_add_form_routes = $this->getAddEntityFormRoutes($entity_type)) {
      foreach ($add_entity_add_form_routes as $entity_type_key => $entity_add_route) {
        $collection->add("$entity_type_key.connection.add_form", $entity_add_route);
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
   * @param \Drupal\Core\Entity\EntityTypeInterface $entity_type
   *   The entity type.
   *
   * @return array
   *   An array of routes, if available.
   */
  protected function getAddEntityPageRoutes($entity_type) {
    $routes = [];

    $entity_type_id = $entity_type->id();

    $entity_types = \Drupal::entityTypeManager()->getDefinitions();

    foreach ($entity_types as $type) {
      if ($canonical = $type->getLinkTemplate('canonical')) {
        $type_id = $type->id();

        if ($type_id == 'redhen_connection_type' || $type_id == 'redhen_connection_role') {
          continue;
        }

        $parameters = [
          $type_id => ['type' => 'entity:'. $type_id],
        ];

        $route = new Route($canonical . "/connection/add");
        $route->setOption('parameters', $parameters);
        $route
          ->setDefaults([
            '_controller' => 'Drupal\redhen_connection\Controller\ConnectionAddController::add',
            '_title' => 'Add ' . $entity_type->getLabel(),
          ])
          ->setRequirement('_entity_create_access', $entity_type_id);
        $routes[$type_id] = $route;
      }
    }

    return $routes;
  }

  /**
   * Gets all entity type add connection routes.
   *
   * @param \Drupal\Core\Entity\EntityTypeInterface $entity_type
   *   The entity type.
   *
   * @return array
   *   An array of routes, if available.
   */
  protected function getAddEntityFormRoutes($entity_type) {
    $routes = [];

    $entity_type_id = $entity_type->id();
    $connection_type_id = $entity_type->getBundleEntityType();

    $entity_types = \Drupal::entityTypeManager()->getDefinitions();

    foreach ($entity_types as $type) {
      if ($canonical = $type->getLinkTemplate('canonical')) {
        $type_id = $type->id();
        if ($type_id == 'redhen_connection_type' || $type_id == 'redhen_connection_role') {
          continue;
        }
        $parameters = [
          $type_id => ['type' => 'entity:'. $type_id],
          $connection_type_id => ['type' => 'entity:' . $connection_type_id]
        ];

        $route = new Route($canonical . "/connection/add/{redhen_connection_type}");
        $route->setOption('parameters', $parameters);
        $route
          ->setDefaults([
            '_controller' => 'Drupal\redhen_connection\Controller\ConnectionAddController::addForm',
            '_title_callback' => 'Drupal\redhen_connection\Controller\ConnectionAddController::getAddFormTitle',
          ])
          ->setRequirement('_entity_create_access', $entity_type_id);
        $routes[$type_id] = $route;
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

}
