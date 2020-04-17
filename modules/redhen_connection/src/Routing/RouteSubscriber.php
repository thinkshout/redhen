<?php

namespace Drupal\redhen_connection\Routing;

use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Routing\RouteSubscriberBase;
use Drupal\Core\Routing\RoutingEvents;
use Symfony\Component\Routing\Route;
use Symfony\Component\Routing\RouteCollection;

/**
 * Listens to the dynamic route events.
 */
class RouteSubscriber extends RouteSubscriberBase {

  /**
   * The entity type manager service.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * Constructs a new RouteSubscriber object.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_manager
   *   The entity type manager.
   */
  public function __construct(EntityTypeManagerInterface $entity_manager) {
    $this->entityTypeManager = $entity_manager;
  }

  /**
   * {@inheritdoc}
   */
  protected function alterRoutes(RouteCollection $collection) {
    foreach ($this->entityTypeManager->getDefinitions() as $entity_type_id => $entity_type) {
      // If the entity didn't get a redhen_connection link template added by
      // hook_entity_types_alter(), skip it.
      if (!($path = $entity_type->getLinkTemplate('redhen_connection'))) {
        continue;
      }

      // Create the "listing" route to show all the connected entities for this
      // entity.
      $route = new Route($path);
      $route
        ->addDefaults([
          '_controller' => '\Drupal\redhen_connection\Controller\RedhenConnections::list',
          '_title' => 'Connections',
        ])
        ->addRequirements([
          '_permission' => 'view active connection entities',
        ])
        ->setOption('_admin_route', TRUE)
        ->setOption('parameters', [
          $entity_type_id => ['type' => 'entity:' . $entity_type_id],
        ]);
      $collection->add("entity.$entity_type_id.redhen_connection", $route);
    }
  }

  /**
   * {@inheritdoc}
   */
  public static function getSubscribedEvents() {
    $events = parent::getSubscribedEvents();
    $events[RoutingEvents::ALTER] = ['onAlterRoutes', 100];
    return $events;
  }

}
