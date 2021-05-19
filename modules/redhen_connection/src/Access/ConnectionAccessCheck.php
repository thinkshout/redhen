<?php

namespace Drupal\redhen_connection\Access;

use Drupal\Core\Access\AccessResult;
use Drupal\Core\Routing\Access\AccessInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\redhen_connection\ConnectionInterface;
use Drupal\redhen_connection\Entity\Connection;
use Drupal\redhen_contact\ContactInterface;
use Drupal\redhen_contact\Entity\Contact;
use Drupal\redhen_org\Entity\Org;
use Drupal\redhen_org\OrgInterface;

class ConnectionAccessCheck implements AccessInterface {

  /**
   * Checks access to the RedHen operation on the given route.
   *
   * The route's '_redhen_connection_access' requirement should specify an
   * operation as a string, where available operations are:
   * 'view', 'view label', 'update', and 'delete'.
   *
   * For example, this route configuration invokes a permissions check for
   * 'update' access to entities of type 'redhen_org':
   * @code
   * pattern: '/foo/{redhen_org}/bar'
   * requirements:
   *   _redhen_connection_access: 'update'
   * @endcode
   *
   * @param \Drupal\Core\Routing\RouteMatchInterface $route_match
   *   The parameterized route.
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The user account.
   *
   * @return \Drupal\Core\Access\AccessResultInterface
   *   The access result.
   */
  public function access(RouteMatchInterface $route_match, AccountInterface $account) {
    $route = $route_match->getRouteObject();
    $operation = $route->getRequirement('_redhen_connection_access');
    $entity = $this->getEntity($route_match);

    if ($entity) {
      return $entity->access($operation, $account, TRUE);
    }

    return AccessResult::neutral();
  }

  /**
   * Get the RedHen entity object from the given route.
   *
   * @param \Drupal\Core\Routing\RouteMatchInterface $route_match
   *   The parameterized route.
   *
   * @return \Drupal\Core\Entity\EntityInterface|null
   *   The entity object or NULL if there is no RedHen parameter in the route.
   */
  protected function getEntity(RouteMatchInterface $route_match) {
    $connection = $route_match->getParameter('redhen_connection');
    if ($connection) {
      if ($connection instanceof ConnectionInterface) {
        return $connection;
      }

      return Connection::load($connection);
    }

    $contact = $route_match->getParameter('redhen_contact');
    if ($contact) {
      if ($contact instanceof ContactInterface) {
        return $contact;
      }

      return Contact::load($contact);
    }

    $org = $route_match->getParameter('redhen_org');
    if ($org) {
      if ($org instanceof OrgInterface) {
        return $org;
      }

      return Org::load($org);
    }

    return NULL;
  }
}
